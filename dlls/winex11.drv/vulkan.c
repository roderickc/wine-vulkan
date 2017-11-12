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
#include "x11drv.h"
#include "wine/vulkan.h"
#include "wine/vulkan_driver.h"

#ifdef SONAME_LIBVULKAN

WINE_DEFAULT_DEBUG_CHANNEL(vulkan);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#endif

typedef VkFlags VkXlibSurfaceCreateFlagsKHR;
#define VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR 1000004000

/* All Vulkan structures use this structure for the first elements. */
struct wine_vk_structure_header
{
    VkStructureType sType;
    const void *pNext;
};

typedef struct VkXlibSurfaceCreateInfoKHR {
    VkStructureType                sType;
    const void*                    pNext;
    VkXlibSurfaceCreateFlagsKHR    flags;
    Display*                       dpy;
    Window                         window;
} VkXlibSurfaceCreateInfoKHR;

static VkResult (*pvkCreateInstance)(const VkInstanceCreateInfo *, const VkAllocationCallbacks *, VkInstance *);
static VkResult (*pvkCreateXlibSurfaceKHR)(VkInstance, const VkXlibSurfaceCreateInfoKHR *, const VkAllocationCallbacks *, VkSurfaceKHR *);
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
LOAD_FUNCPTR(vkCreateXlibSurfaceKHR)
LOAD_FUNCPTR(vkDestroyInstance)
LOAD_FUNCPTR(vkGetDeviceProcAddr)
LOAD_FUNCPTR(vkGetInstanceProcAddr)
#undef LOAD_FUNCPTR

    return TRUE;
}

/* Helper function for converting between win32 and X11 compatible VkInstanceCreateInfo.
 * Caller is responsible for allocation and cleanup of 'dst'.
 */
static VkResult wine_vk_instance_convert_create_info(const VkInstanceCreateInfo *src, VkInstanceCreateInfo *dst)
{
    int i, size;
    const char **enabled_extensions = NULL;

    dst->sType = src->sType;
    dst->flags = src->flags;
    dst->pApplicationInfo = src->pApplicationInfo;

    /* Application + loader can pass in a chain of extensions through pNext e.g. VK_EXT_debug_report
     * and layers (not sure why loader doesn't filter out loaders to ICD). We need to see how to handle
     * these as we can't just blindly pass structures through as some like VK_EXT_debug_report have
     * callbacks. Mesa ANV / Radv are ignoring pNext at the moment, unclear what binary blobs do.
     * Since in our case we are going through the Linux vulkan loader, the loader itself will add
     * some duplicate layers, so for now it is probably best to ignore extra extensions.
     */
    if (src->pNext)
    {
        const struct wine_vk_structure_header *header;

        for (header = src->pNext; header; header = header->pNext)
        {
            FIXME("Application requested a linked structure of type %d\n", header->sType);
        }
    }
    /* For now don't support anything. */
    dst->pNext = NULL;

    /* ICDs don't support any layers (at least at time of writing). The loader seems to not
     * filter out layer information when it reaches us. To avoid confusion by the native loader
     * we should filter.
     */
    dst->enabledLayerCount = 0;
    dst->ppEnabledLayerNames = NULL;

    if (src->enabledExtensionCount > 0)
    {
        size = src->enabledExtensionCount * sizeof(*src->ppEnabledExtensionNames);
        enabled_extensions = HeapAlloc(GetProcessHeap(), 0, size);
        if (!enabled_extensions)
        {
            ERR("Failed to allocate memory for enabled extensions\n");
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }

        for (i = 0; i < src->enabledExtensionCount; i++)
        {
            /* Substitute extension with X11 else copy. Long-term when we support more
             * extenions we should store these translations in a list.
             */
            if (!strcmp(src->ppEnabledExtensionNames[i], "VK_KHR_win32_surface"))
            {
                enabled_extensions[i] = "VK_KHR_xlib_surface";
            }
            else
            {
                enabled_extensions[i] = src->ppEnabledExtensionNames[i];
            }
        }
        dst->ppEnabledExtensionNames = (const char**)enabled_extensions;
    }
    dst->enabledExtensionCount = src->enabledExtensionCount;

    return VK_SUCCESS;
}

static VkResult X11DRV_vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
        VkInstance *pInstance)
{
    VkInstanceCreateInfo create_info;
    VkResult res;

    TRACE("pCreateInfo %p, pAllocater %p, pInstance %p\n", pCreateInfo, pAllocator, pInstance);

    if (pAllocator)
    {
        FIXME("Support for allocation callbacks not implemented yet\n");
    }

    res = wine_vk_instance_convert_create_info(pCreateInfo, &create_info);
    if (res != VK_SUCCESS)
    {
        ERR("Failed to convert instance create info, res=%d\n", res);
        return res;
    }

    res = pvkCreateInstance(&create_info, NULL /* pAllocator */, pInstance);

    if (create_info.ppEnabledExtensionNames)
        HeapFree(GetProcessHeap(), 0, (void*)create_info.ppEnabledExtensionNames);

    return res;
}

static VkResult X11DRV_vkCreateWin32SurfaceKHR(VkInstance instance, const VkWin32SurfaceCreateInfoKHR *pCreateInfo,
        const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface)
{
    VkResult res;
    VkXlibSurfaceCreateInfoKHR create_info;
    TRACE("%p %p %p %p\n", instance, pCreateInfo, pAllocator, pSurface);

    if (pAllocator)
        FIXME("Support for allocation callbacks not implemented yet\n");

    create_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    create_info.pNext = NULL;
    create_info.flags = 0; /* reserved */
    create_info.dpy = gdi_display;
    /* This is just a temporary hack to get some kind of X11 window.
     * The code won't be submitted to Wine like this.
     * Long-term this call may become more complicated due to child
     * window rendering. Vulkan allows for this, but I have not encounted
     * any application using it yet. */
    create_info.window = (Window)(DWORD_PTR)GetPropA( pCreateInfo->hwnd, "__wine_x11_whole_window" );

    res = pvkCreateXlibSurfaceKHR(instance, &create_info, NULL /* pAllocator */, pSurface);
    if (res != VK_SUCCESS)
        return res;

    TRACE("Created surface=0x%s\n", wine_dbgstr_longlong(*pSurface));
    return VK_SUCCESS;
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
