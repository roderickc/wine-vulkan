/* Wine Vulkan ICD implementation
 *
 * Copyright 2017 Roderick Colenbrander
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdarg.h>

#include "windef.h"
#include "winbase.h"
#include "winuser.h"

#include "wine/debug.h"
#include "wine/vulkan.h"
#include "wine/vulkan_driver.h"
#include "vulkan_private.h"

WINE_DEFAULT_DEBUG_CHANNEL(vulkan);

/* For now default to 3 as it felt like a reasonable version feature wise to support.
 * Version 4 requires us to implement vk_icdGetPhysicalDeviceProcAddr, which I didn't
 * want to deal with just yet. It also added some more detailed API version check
 * requirements. Version 5 builds further on this. Something to tackle later.
 */
#define WINE_VULKAN_ICD_VERSION 3

static VkResult WINAPI wine_vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
        VkInstance *pInstance);
static VkResult WINAPI wine_vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pPropertyCount,
        VkExtensionProperties* pProperties);
static PFN_vkVoidFunction WINAPI wine_vkGetInstanceProcAddr(VkInstance instance, const char* pName);

const struct vulkan_func vk_global_dispatch_table[] = {
    {"vkCreateInstance", &wine_vkCreateInstance},
    {"vkEnumerateInstanceExtensionProperties", &wine_vkEnumerateInstanceExtensionProperties},
    {"vkGetInstanceProcAddr", &wine_vkGetInstanceProcAddr},
};

static struct vulkan_funcs *vk_funcs = NULL;

static void *wine_vk_alloc_dispatchable_object(size_t size)
{
    struct wine_vk_dispatchable_object *object = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);

    if (!object)
        return NULL;

    /* Set special header for ICD loader. */
    ((struct wine_vk_base*)object)->loader_magic = VULKAN_ICD_MAGIC_VALUE;

    return object;
}

static void *wine_vk_get_global_proc_addr(const char *name)
{
    int i;
    for (i = 0; i < sizeof(vk_global_dispatch_table) / sizeof(vk_global_dispatch_table[0]); i++)
    {
        if (strcmp(name, vk_global_dispatch_table[i].name) == 0)
        {
            TRACE("Found pName=%s in global table\n", name);
            return vk_global_dispatch_table[i].func;
        }
    }
    return NULL;
}

static BOOL wine_vk_init(HINSTANCE hinst)
{
    HDC hdc = GetDC(0);

    vk_funcs =  __wine_get_vulkan_driver(hdc, WINE_VULKAN_DRIVER_VERSION);
    if (!vk_funcs)
    {
        ReleaseDC(0, hdc);
        return FALSE;
    }

    DisableThreadLibraryCalls(hinst);

    ReleaseDC(0, hdc);
    return TRUE;
}

/* Helper function which stores wrapped physical devices in the instance object. */
static VkResult wine_vk_instance_load_physical_devices(struct VkInstance_T *instance)
{
    VkResult res;
    struct VkPhysicalDevice_T **tmp_phys_devs = NULL;
    int i;
    uint32_t num_phys_devs = 0;

    res = instance->funcs.p_vkEnumeratePhysicalDevices(instance->instance, &num_phys_devs, NULL);
    if (res != VK_SUCCESS)
    {
        ERR("Failed to enumerate physical devices, res=%d\n", res);
        return res;
    }

    /* Don't bother with any of the rest if the system just lacks devices. */
    if (num_phys_devs == 0)
    {
        instance->num_phys_devs = 0;
        instance->phys_devs_initialized = TRUE;
        return VK_SUCCESS;
    }

    tmp_phys_devs = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, num_phys_devs * sizeof(*tmp_phys_devs));
    if (!tmp_phys_devs)
        return VK_ERROR_OUT_OF_HOST_MEMORY;

    res = instance->funcs.p_vkEnumeratePhysicalDevices(instance->instance, &num_phys_devs, tmp_phys_devs);
    if (res != VK_SUCCESS)
        goto err;

    instance->phys_devs = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, num_phys_devs * sizeof(*instance->phys_devs));
    if (!instance->phys_devs)
    {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto err;
    }

    /* Wrap each native physical device handle into a dispatchable object for the ICD loader. */
    for (i = 0; i < num_phys_devs; i++)
    {
        VkPhysicalDevice phys_dev = wine_vk_alloc_dispatchable_object(sizeof(*phys_dev));
        if (!phys_dev)
        {
            ERR("Unable to allocate memory for physical device!\n");
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto err;
        }

        phys_dev->instance = instance;
        phys_dev->phys_dev = tmp_phys_devs[i];

        instance->phys_devs[i] = phys_dev;
        instance->num_phys_devs = i;
    }
    instance->num_phys_devs = num_phys_devs;
    instance->phys_devs_initialized = TRUE;

    HeapFree(GetProcessHeap(), 0, tmp_phys_devs);
    return VK_SUCCESS;

err:
    if (tmp_phys_devs)
        HeapFree(GetProcessHeap(), 0, tmp_phys_devs);

    if (instance->phys_devs)
    {
        for (i = 0; i < instance->num_phys_devs; i++)
        {
            HeapFree(GetProcessHeap(), 0, instance->phys_devs[i]);
            instance->phys_devs[i] = NULL;
        }
        HeapFree(GetProcessHeap(), 0, instance->phys_devs);
        instance->num_phys_devs = 0;
        instance->phys_devs = NULL;
        instance->phys_devs_initialized = FALSE;
    }

    return res;
}

/* Helper function used for freeing an instance structure. This function supports full
 * and partial object cleanups and can thus be used for vkCreateInstance failures.
 */
static void wine_vk_instance_free(struct VkInstance_T *instance)
{
    if (!instance)
        return;

    if (instance->phys_devs)
    {
        int i;

        for (i = 0; i < instance->num_phys_devs; i++)
        {
            HeapFree(GetProcessHeap(), 0, &instance->phys_devs[i]);
        }
        HeapFree(GetProcessHeap(), 0, instance->phys_devs);
    }

    if (instance->instance)
        vk_funcs->p_vkDestroyInstance(instance->instance, NULL /* pAllocator */);

    HeapFree(GetProcessHeap(), 0, instance);
}

static VkResult WINAPI wine_vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
        VkInstance *pInstance)
{
    VkInstance instance = NULL;
    VkResult res;

    TRACE("pCreateInfo %p, pAllocater %p, pInstance %p\n", pCreateInfo, pAllocator, pInstance);

    if (pAllocator)
        FIXME("Support for allocation callbacks not implemented yet\n");

    instance = wine_vk_alloc_dispatchable_object(sizeof(*instance));
    if (!instance)
    {
        ERR("Failed to allocate memory for instance\n");
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto err;
    }

    res = vk_funcs->p_vkCreateInstance(pCreateInfo, NULL /* pAllocator */, &instance->instance);
    if (res != VK_SUCCESS)
    {
        ERR("Failed to create instance, res=%d\n", res);
        goto err;
    }

    /* Load all instance functions we are aware of. Note the loader takes care
     * of any filtering for extensions which were not requested, but which the
     * ICD may support.
     */
#define USE_VK_FUNC(name) \
    instance->funcs.p_##name = (void*)vk_funcs->p_vkGetInstanceProcAddr(instance->instance, #name);
    ALL_VK_INSTANCE_FUNCS()
#undef USE_VK_FUNC

    instance->phys_devs_initialized = FALSE;

    *pInstance = instance;
    TRACE("Done, instance=%p native_instance=%p\n", instance, instance->instance);
    return VK_SUCCESS;

err:
    if (instance)
        wine_vk_instance_free(instance);

    return res;
}

void WINAPI wine_vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, %p\n", instance, pAllocator);

    if (pAllocator)
        FIXME("Support allocation allocators\n");

    wine_vk_instance_free(instance);
}

static VkResult WINAPI wine_vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pPropertyCount,
        VkExtensionProperties *pProperties)
{
    TRACE("%p %p %p\n", pLayerName, pPropertyCount, pProperties);
    return vk_funcs->p_vkEnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
}

VkResult WINAPI wine_vkEnumeratePhysicalDevices(VkInstance instance, uint32_t *pPhysicalDeviceCount,
        VkPhysicalDevice *pPhysicalDevices)
{
    VkResult res;
    int i;
    int num_copies = 0;

    TRACE("%p %p %p\n", instance, pPhysicalDeviceCount, pPhysicalDevices);

    /* Cache physical devices for vkEnumeratePhysicalDevices within the instance as each
     * vkPhysicalDevice is a dispatchable object, which means we need to wrap the native
     * physical device and present those the application. Applications call this function
     * multiple times first to get the number of devices, then to get the devices.
     * Cleanup happens as part of wine_vkDestroyInstance.
     */
    if (instance->phys_devs_initialized == FALSE)
    {
        res = wine_vk_instance_load_physical_devices(instance);
        if (res != VK_SUCCESS)
        {
            ERR("Failed to cache physical devices, res=%d\n", res);
            return res;
        }
    }

    if (!pPhysicalDevices)
    {
        *pPhysicalDeviceCount = instance->num_phys_devs;
        return VK_SUCCESS;
    }

    if (*pPhysicalDeviceCount < instance->num_phys_devs)
    {
        /* Incomplete is a type of success used to signal the application
         * that not all devices got copied.
         */
        num_copies = *pPhysicalDeviceCount;
        res = VK_INCOMPLETE;
    }
    else
    {
        num_copies = instance->num_phys_devs;
        res = VK_SUCCESS;
    }

    for (i = 0; i < num_copies; i++)
    {
        pPhysicalDevices[i] = instance->phys_devs[i];
    }
    *pPhysicalDeviceCount = num_copies;

    TRACE("Returning %d devices\n", *pPhysicalDeviceCount);
    return res;
}

static PFN_vkVoidFunction WINAPI wine_vkGetInstanceProcAddr(VkInstance instance, const char *pName)
{
    void *func;
    TRACE("%p %s\n", instance, debugstr_a(pName));

    /* vkGetInstanceProcAddr can load most Vulkan functions when an instance is passed in, however
     * for a NULL instance it can only load global functions.
     */
    func = wine_vk_get_global_proc_addr(pName);
    if (func)
    {
        return func;
    }
    else if (!instance && !func)
    {
        FIXME("Global function '%s' not found\n", pName);
        return NULL;
    }

    func = wine_vk_get_instance_proc_addr(pName);
    if (func) return func;

    /* vkGetInstanceProcAddr also loads any children of instance, so device functions as well. */
    func = wine_vk_get_device_proc_addr(pName);
    if (func) return func;

    FIXME("Unsupported device or instance function: '%s'\n", pName);
    return NULL;
}

void * WINAPI wine_vk_icdGetInstanceProcAddr(VkInstance instance, const char *pName)
{
    TRACE("%p %s\n", instance, debugstr_a(pName));

    /* Initial version of the Vulkan ICD spec required vkGetInstanceProcAddr to be
     * exported. vk_icdGetInstanceProcAddr was added later to separete ICD calls from
     * Vulkan API. One of them in our case should forward to the other, so just forward
     * to the older vkGetInstanceProcAddr.
     */
    return wine_vkGetInstanceProcAddr(instance, pName);
}

VkResult WINAPI wine_vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t *pSupportedVersion)
{
    uint32_t req_version;
    TRACE("%p\n", pSupportedVersion);

    /* The spec is not clear how to handle this. Mesa drivers don't check, but it
     * is probably best to not explode. VK_INCOMPLETE seems to be the closest value.
     */
    if (!pSupportedVersion)
        return VK_INCOMPLETE;

    req_version = *pSupportedVersion;
    *pSupportedVersion = min(req_version, WINE_VULKAN_ICD_VERSION);
    TRACE("Loader requested ICD version=%d, returning %d\n", req_version, *pSupportedVersion);

    return VK_SUCCESS;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
{
    switch(reason)
    {
        case DLL_PROCESS_ATTACH:
            return wine_vk_init(hinst);

        case DLL_THREAD_ATTACH:
            break;
    }
    return TRUE;
}
