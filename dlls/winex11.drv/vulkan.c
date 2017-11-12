/* X11DRV Vulkan implementation
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

#include "config.h"
#include "wine/port.h"

#include <stdarg.h>

#include "windef.h"
#include "winbase.h"

#include "wine/debug.h"
#include "wine/library.h"
#include "wine/vulkan.h"
#include "wine/vulkan_driver.h"

#ifdef SONAME_LIBVULKAN

WINE_DEFAULT_DEBUG_CHANNEL(vulkan);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#endif

static VkResult (*pvkCreateInstance)(const VkInstanceCreateInfo *, const VkAllocationCallbacks *, VkInstance *);
static void (*pvkDestroyInstance)(VkInstance, const VkAllocationCallbacks *);
static void * (*pvkGetDeviceProcAddr)(VkDevice, const char *);
static void * (*pvkGetInstanceProcAddr)(VkInstance, const char *);

struct VkExtensionProperties winex11_vk_instance_extensions[] = {
    { "VK_KHR_surface", 1 },
    { "VK_KHR_win32_surface", 1},
};

static BOOL wine_vk_init(void)
{
    static BOOL init_done = FALSE;
    static void *vulkan_handle;

    if (init_done) return (vulkan_handle != NULL);
    init_done = TRUE;

    if (!(vulkan_handle = wine_dlopen(SONAME_LIBVULKAN, RTLD_NOW, NULL, 0))) return FALSE;

#define LOAD_FUNCPTR(f) if((p##f = wine_dlsym(vulkan_handle, #f, NULL, 0)) == NULL) return FALSE;
LOAD_FUNCPTR(vkCreateInstance)
LOAD_FUNCPTR(vkDestroyInstance)
LOAD_FUNCPTR(vkGetDeviceProcAddr)
LOAD_FUNCPTR(vkGetInstanceProcAddr)
#undef LOAD_FUNCPTR

    return TRUE;
}

static VkResult X11DRV_vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
        VkInstance *pInstance)
{
    TRACE("pCreateInfo %p, pAllocater %p, pInstance %p\n", pCreateInfo, pAllocator, pInstance);

    if (pAllocator)
        FIXME("Support for allocation callbacks not implemented yet\n");

    /* TODO: convert win32 to x11 extensions here. */
    if (pCreateInfo->enabledExtensionCount > 0)
    {
        FIXME("Extensions are not supported yet, aborting!\n");
        return VK_ERROR_INCOMPATIBLE_DRIVER;
    }

    return pvkCreateInstance(pCreateInfo, NULL /* pAllocator */, pInstance);
}

static VkResult X11DRV_vkCreateWin32SurfaceKHR(VkInstance instance, const VkWin32SurfaceCreateInfoKHR *pCreateInfo,
        const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface)
{
    FIXME("stub: %p %p %p %p\n", instance, pCreateInfo, pAllocator, pSurface);
    return VK_ERROR_OUT_OF_HOST_MEMORY;
}

static void X11DRV_vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p %p\n", instance, pAllocator);

    if (pAllocator)
        FIXME("Support for allocation callbacks not implemented yet\n");

    pvkDestroyInstance(instance, NULL /* pAllocator */);
}

static void X11DRV_vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks *pAllocator)
{
    FIXME("stub: %p 0x%s %p\n", instance, wine_dbgstr_longlong(surface), pAllocator);
}

static VkResult X11DRV_vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pPropertyCount,
        VkExtensionProperties* pProperties)
{
    VkResult res;
    int i, num_copies;

    TRACE("pLayerName %p, pPropertyCount %p, pProperties %p\n", pLayerName, pPropertyCount, pProperties);

    /* This shouldn't get called with pLayerName set, the ICD loader prevents it. */
    if (pLayerName)
    {
        ERR("Layer enumeration not supported from ICD.\n");
        return VK_ERROR_LAYER_NOT_PRESENT;
    }

    if (!pProperties)
    {
        /* For now we only report surface extensions, long-term this needs to be
         * an intersection between what the native library supports and what thunks
         * we have.
         */
        *pPropertyCount = ARRAY_SIZE(winex11_vk_instance_extensions);
        return VK_SUCCESS;
    }

    if (*pPropertyCount < ARRAY_SIZE(winex11_vk_instance_extensions))
    {
        /* Incomplete is a type of success used to signal the application
         * that not all devices got copied.
         */
        num_copies = *pPropertyCount;
        res = VK_INCOMPLETE;
    }
    else
    {
        num_copies = ARRAY_SIZE(winex11_vk_instance_extensions);
        res = VK_SUCCESS;
    }

    for (i = 0; i < num_copies; i++)
    {
        memcpy(&pProperties[i], &winex11_vk_instance_extensions[i], sizeof(winex11_vk_instance_extensions[i]));
    }

    TRACE("Result %d, extensions copied %d\n", res, num_copies);
    return res;
}

static void * X11DRV_vkGetDeviceProcAddr(VkDevice device, const char *pName)
{
    TRACE("%p, %s\n", device, debugstr_a(pName));
    return pvkGetDeviceProcAddr(device, pName);
}

static void * X11DRV_vkGetInstanceProcAddr(VkInstance instance, const char *pName)
{
    TRACE("%p, %s\n", instance, debugstr_a(pName));
    return pvkGetInstanceProcAddr(instance, pName);
}

static VkBool32 X11DRV_vkGetPhysicalDeviceWin32PresentationSupportKHR(VkPhysicalDevice physicalDevice,
        uint32_t queueFamilyIndex)
{
    FIXME("stub %p %u\n", physicalDevice, queueFamilyIndex);
    return VK_FALSE;
}

static struct vulkan_funcs vulkan_funcs = {
    X11DRV_vkCreateInstance,
    X11DRV_vkCreateWin32SurfaceKHR,
    X11DRV_vkDestroyInstance,
    X11DRV_vkDestroySurfaceKHR,
    X11DRV_vkEnumerateInstanceExtensionProperties,
    X11DRV_vkGetDeviceProcAddr,
    X11DRV_vkGetInstanceProcAddr,
    X11DRV_vkGetPhysicalDeviceWin32PresentationSupportKHR
};

struct vulkan_funcs *get_vulkan_driver(UINT version)
{
    if (version != WINE_VULKAN_DRIVER_VERSION)
    {
        ERR("version mismatch, vulkan wants %u but driver has %u\n", version, WINE_VULKAN_DRIVER_VERSION);
        return NULL;
    }

    if (wine_vk_init())
        return &vulkan_funcs;

    return NULL;
}

#else /* No vulkan */

struct vulkan_funcs *get_vulkan_driver(UINT version)
{
    return NULL;
}

#endif /* SONAME_LIBVULKAN */
