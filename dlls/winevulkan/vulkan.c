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

/* Helper function used for freeing a device structure. This function supports full
 * and partial object cleanups and can thus be used vkCreateDevice failures.
 */
static void wine_vk_device_free(struct VkDevice_T *device)
{
    if (!device)
        return;

    if (device->queues)
    {
        int i;
        for (i = 0; i < device->max_queue_families; i++)
        {
            if (device->queues[i])
                HeapFree(GetProcessHeap(), 0, device->queues[i]);
        }
        HeapFree(GetProcessHeap(), 0, device->queues);
        device->queues = NULL;
    }

    if (device->queue_count)
        HeapFree(GetProcessHeap(), 0, device->queue_count);

    if (device->device && device->funcs.p_vkDestroyDevice)
    {
        device->funcs.p_vkDestroyDevice(device->device, NULL /* pAllocator */);
    }

    HeapFree(GetProcessHeap(), 0, device);
}

/* Helper function for release command buffers. */
static void wine_vk_device_free_command_buffers(VkDevice device, VkCommandPool pool, uint32_t count, const VkCommandBuffer *buffers)
{
    int i;

    /* To avoid have to wrap all command buffers just loop over them one by one. */
    for (i = 0; i < count; i++)
    {
        if (buffers[i]->command_buffer)
            device->funcs.p_vkFreeCommandBuffers(device->device, pool, 1, &buffers[i]->command_buffer);

        HeapFree(GetProcessHeap(), 0, buffers[i]);
    }
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

/* Helper function to create queues for a given family index. */
static struct VkQueue_T *wine_vk_device_alloc_queues(struct VkDevice_T *device, uint32_t fam_index, uint32_t queue_count)
{
    int i;

    struct VkQueue_T *queues = HeapAlloc(GetProcessHeap(), 0, sizeof(struct VkQueue_T)*queue_count);
    if (!queues)
    {
        ERR("Failed to allocate memory for queues\n");
        return NULL;
    }

    for (i = 0; i < queue_count; i++)
    {
        struct VkQueue_T *queue = &queues[i];
        queue->device = device;

        /* The native device was already allocated with the required number of queues, 
         * so just fetch them from there.
         */
        device->funcs.p_vkGetDeviceQueue(device->device, fam_index, i, &queue->queue);

        /* Set special header for ICD loader. */
        ((struct wine_vk_base*)queue)->loader_magic = VULKAN_ICD_MAGIC_VALUE;
    }

    return queues;
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

VkResult WINAPI wine_vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo *pAllocateInfo,
        VkCommandBuffer *pCommandBuffers)
{
    VkResult res = VK_SUCCESS;
    int i;

    TRACE("%p %p %p\n", device, pAllocateInfo, pCommandBuffers);

    /* The application provides an array of buffers, we just clear it for error handling reasons. */
    memset(pCommandBuffers, 0, sizeof(*pCommandBuffers)*pAllocateInfo->commandBufferCount);

    for (i = 0; i < pAllocateInfo->commandBufferCount; i++)
    {
        VkCommandBufferAllocateInfo allocate_info;
        allocate_info.commandPool = pAllocateInfo->commandPool;
        allocate_info.level = pAllocateInfo->level;
        allocate_info.commandBufferCount = 1;

        TRACE("Creating command buffer %d, pool 0x%s, level %d\n", i, wine_dbgstr_longlong(allocate_info.commandPool),
                allocate_info.level);
        pCommandBuffers[i] = wine_vk_alloc_dispatchable_object(sizeof(struct VkCommandBuffer_T));
        if (!pCommandBuffers[i])
        {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }

        pCommandBuffers[i]->device = device;
        res = device->funcs.p_vkAllocateCommandBuffers(device->device, &allocate_info, &pCommandBuffers[i]->command_buffer);
        if (res != VK_SUCCESS)
        {
            ERR("Failed to allocate command buffer, res=%d\n", res);
            break;
        }
    }

    if (res != VK_SUCCESS)
    {
        wine_vk_device_free_command_buffers(device, pAllocateInfo->commandPool, i, pCommandBuffers);
        return res;
    }

    return VK_SUCCESS;
}

VkResult WINAPI wine_vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo *pCreateInfo,
        const VkAllocationCallbacks *pAllocator, VkDevice *pDevice)
{
    struct VkDevice_T *device = NULL;
    uint32_t max_queue_families;
    VkResult res;
    int i;

    TRACE("%p %p %p %p\n", physicalDevice, pCreateInfo, pAllocator, pDevice);

    if (pAllocator)
    {
        FIXME("Support for allocation callbacks not implemented yet\n");
    }

    device = wine_vk_alloc_dispatchable_object(sizeof(*device));
    if (!device)
        return VK_ERROR_OUT_OF_HOST_MEMORY;

    /* At least for now we can directly pass pCreateInfo through. All extensions we report
     * should be compatible. In addition the loader is supposed to santize values e.g. layers.
     */
    res = physicalDevice->instance->funcs.p_vkCreateDevice(physicalDevice->phys_dev, pCreateInfo, NULL /* pAllocator */,
            &device->device);
    if (res != VK_SUCCESS)
    {
        ERR("Failed to create device\n");
        goto err;
    }

    device->phys_dev = physicalDevice;

    /* Just load all function pointers we are aware off. The loader takes care of filtering.
     * We use vkGetDeviceProcAddr as opposed to vkGetInstanceProcAddr for efficiency reasons
     * as functions pass through fewer dispatch tables within the loader.
     */
#define USE_VK_FUNC(name) \
    device->funcs.p_##name = (void*)vk_funcs->p_vkGetDeviceProcAddr(device->device, #name); \
    if (device->funcs.p_##name == NULL) \
        TRACE("Not found %s\n", #name);
    ALL_VK_DEVICE_FUNCS()
#undef USE_VK_FUNC

    /* We need to cache all queues within the device as each requires wrapping since queues are
     * dispatchable objects.
     */
    physicalDevice->instance->funcs.p_vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice->phys_dev,
            &max_queue_families, NULL);
    device->max_queue_families = max_queue_families;
    TRACE("Max queue families: %d\n", device->max_queue_families);

    device->queues = HeapAlloc(GetProcessHeap(), 0, sizeof(*device->queues)*max_queue_families);
    if (!device->queues)
    {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto err;
    }

    device->queue_count = HeapAlloc(GetProcessHeap(), 0, sizeof(*device->queue_count)*max_queue_families);
    if (!device->queue_count)
    {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto err;
    }

    for (i = 0; i < pCreateInfo->queueCreateInfoCount; i++)
    {
        uint32_t fam_index = pCreateInfo->pQueueCreateInfos[i].queueFamilyIndex;
        uint32_t queue_count = pCreateInfo->pQueueCreateInfos[i].queueCount;

        TRACE("queueFamilyIndex %d, queueCount %d\n", fam_index, queue_count);

        device->queues[fam_index] = wine_vk_device_alloc_queues(device, fam_index, queue_count);
        if (!device->queues[fam_index])
        {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            ERR("Failed to allocate memory for queues\n");
            goto err;
        }
        device->queue_count[fam_index] = queue_count;
    }


    *pDevice = device;
    return VK_SUCCESS;

err:
    wine_vk_device_free(device);
    return res;
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

VkResult WINAPI wine_vkCreateWin32SurfaceKHR(VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
        const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface)
{
    TRACE(" %p %p %p %p\n", instance, pCreateInfo, pAllocator, pSurface);

    if (pAllocator)
        FIXME("Support allocation allocators\n");

    return vk_funcs->p_vkCreateWin32SurfaceKHR(instance->instance, pCreateInfo, NULL /* pAllocator */, pSurface);
}

void WINAPI wine_vkDestroyDevice(VkDevice device, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p %p\n", device, pAllocator);

    if (pAllocator)
    {
        FIXME("Support for allocation callbacks not implemented yet\n");
    }

    wine_vk_device_free(device);
}

void WINAPI wine_vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, %p\n", instance, pAllocator);

    if (pAllocator)
        FIXME("Support allocation allocators\n");

    wine_vk_instance_free(instance);
}

void WINAPI wine_vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", instance, wine_dbgstr_longlong(surface), pAllocator);

    if (pAllocator)
        FIXME("Support allocation allocators\n");

    return vk_funcs->p_vkDestroySurfaceKHR(instance->instance, surface, pAllocator);
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

void WINAPI wine_vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount,
        const VkCommandBuffer *pCommandBuffers)
{
    TRACE("%p 0x%s %d %p\n", device, wine_dbgstr_longlong(commandPool), commandBufferCount, pCommandBuffers);

    wine_vk_device_free_command_buffers(device, commandPool, commandBufferCount, pCommandBuffers);
}

PFN_vkVoidFunction WINAPI wine_vkGetDeviceProcAddr(VkDevice device, const char *pName)
{
    void *func;
    TRACE("%p, %s\n", device, debugstr_a(pName));

    /* The spec leaves return value undefined for a NULL device, let's just return NULL. */
    if (!device)
        return NULL;

    /* Per the spec, we are only supposed to return device functions as in functions
     * for which the first parameter is vkDevice or a child of vkDevice like a
     * vkCommanBuffer, vkQueue.
     * Loader takes are of filtering of extensions which are enabled or not.
     */
    func = wine_vk_get_device_proc_addr(pName);
    if (func)
        return func;

    TRACE("Function %s not found\n", pName);
    return NULL;
}

void WINAPI wine_vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue *pQueue)
{
    TRACE("%p %d %d %p\n", device, queueFamilyIndex, queueIndex, pQueue);

    *pQueue = &device->queues[queueFamilyIndex][queueIndex];
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

VkBool32 WINAPI wine_vkGetPhysicalDeviceWin32PresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
{
    TRACE("%p %u\n", physicalDevice, queueFamilyIndex);
    return vk_funcs->p_vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice->phys_dev, queueFamilyIndex);
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

VkResult WINAPI wine_vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo *pSubmits, VkFence fence)
{
    VkSubmitInfo *submits;
    VkResult res;
    VkCommandBuffer *command_buffers;
    int i, num_command_buffers;

    TRACE("%p %u %p 0x%s\n", queue, submitCount, pSubmits, wine_dbgstr_longlong(fence));

    if (submitCount == 0)
    {
        return queue->device->funcs.p_vkQueueSubmit(queue->queue, 0, NULL, fence);
    }

    submits = HeapAlloc(GetProcessHeap(), 0, sizeof(*submits)*submitCount);
    if (!submits)
    {
        ERR("Unable to allocate memory for submit buffers!\n");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (i = 0; i < submitCount; i++)
    {
        int j;

        memcpy(&submits[i], &pSubmits[i], sizeof(*submits));

        num_command_buffers = pSubmits[i].commandBufferCount;
        command_buffers = HeapAlloc(GetProcessHeap(), 0, sizeof(*submits)*num_command_buffers);
        if (!command_buffers)
        {
            ERR("Unable to allocate memory for comman buffers!\n");
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto err;
        }

        for (j = 0; j < num_command_buffers; j++)
        {
            command_buffers[j] = pSubmits[i].pCommandBuffers[j]->command_buffer;
        }
        submits[i].pCommandBuffers = command_buffers;
    }

    res = queue->device->funcs.p_vkQueueSubmit(queue->queue, submitCount, submits, fence);

err:
    if (submits)
    {
        for (i = 0; i < submitCount; i++)
        {
            if (submits[i].pCommandBuffers)
                HeapFree(GetProcessHeap(), 0, (void*)submits[i].pCommandBuffers);
        }
        HeapFree(GetProcessHeap(), 0, submits);
    }

    TRACE("Returning %d\n", res);
    return res;
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
