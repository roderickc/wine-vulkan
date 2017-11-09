/* Automatically generated from Vulkan vk.xml; DO NOT EDIT! */

#include "config.h"
#include "wine/port.h"

#include "wine/debug.h"
#include "wine/vulkan.h"
#include "wine/vulkan_driver.h"
#include "vulkan_private.h"

WINE_DEFAULT_DEBUG_CHANNEL(vulkan);

static VkResult WINAPI wine_vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDevice *pDevice)
{
    FIXME("stub: %p, %p, %p, %p\n", physicalDevice, pCreateInfo, pAllocator, pDevice);
    return VK_ERROR_OUT_OF_HOST_MEMORY;
}

static VkResult WINAPI wine_vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char *pLayerName, uint32_t *pPropertyCount, VkExtensionProperties *pProperties)
{
    TRACE("%p, %p, %p, %p\n", physicalDevice, pLayerName, pPropertyCount, pProperties);
    return physicalDevice->instance->funcs.p_vkEnumerateDeviceExtensionProperties(physicalDevice->phys_dev, pLayerName, pPropertyCount, pProperties);
}

static VkResult WINAPI wine_vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t *pPropertyCount, VkLayerProperties *pProperties)
{
    TRACE("%p, %p, %p\n", physicalDevice, pPropertyCount, pProperties);
    return physicalDevice->instance->funcs.p_vkEnumerateDeviceLayerProperties(physicalDevice->phys_dev, pPropertyCount, pProperties);
}

static void WINAPI wine_vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures *pFeatures)
{
    TRACE("%p, %p\n", physicalDevice, pFeatures);
    physicalDevice->instance->funcs.p_vkGetPhysicalDeviceFeatures(physicalDevice->phys_dev, pFeatures);
}

static void WINAPI wine_vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties *pFormatProperties)
{
    TRACE("%p, %d, %p\n", physicalDevice, format, pFormatProperties);
    physicalDevice->instance->funcs.p_vkGetPhysicalDeviceFormatProperties(physicalDevice->phys_dev, format, pFormatProperties);
}

static VkResult WINAPI wine_vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties *pImageFormatProperties)
{
    TRACE("%p, %d, %d, %d, %#x, %#x, %p\n", physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
    return physicalDevice->instance->funcs.p_vkGetPhysicalDeviceImageFormatProperties(physicalDevice->phys_dev, format, type, tiling, usage, flags, pImageFormatProperties);
}

static void WINAPI wine_vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties *pMemoryProperties)
{
    TRACE("%p, %p\n", physicalDevice, pMemoryProperties);
    physicalDevice->instance->funcs.p_vkGetPhysicalDeviceMemoryProperties(physicalDevice->phys_dev, pMemoryProperties);
}

static void WINAPI wine_vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties *pProperties)
{
    TRACE("%p, %p\n", physicalDevice, pProperties);
    physicalDevice->instance->funcs.p_vkGetPhysicalDeviceProperties(physicalDevice->phys_dev, pProperties);
}

static void WINAPI wine_vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t *pQueueFamilyPropertyCount, VkQueueFamilyProperties *pQueueFamilyProperties)
{
    TRACE("%p, %p, %p\n", physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    physicalDevice->instance->funcs.p_vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice->phys_dev, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

static void WINAPI wine_vkGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t *pPropertyCount, VkSparseImageFormatProperties *pProperties)
{
    TRACE("%p, %d, %d, %d, %#x, %d, %p, %p\n", physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
    physicalDevice->instance->funcs.p_vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevice->phys_dev, format, type, samples, usage, tiling, pPropertyCount, pProperties);
}

static const struct vulkan_func vk_instance_dispatch_table[] = {
    {"vkCreateDevice", &wine_vkCreateDevice},
    {"vkDestroyInstance", &wine_vkDestroyInstance},
    {"vkEnumerateDeviceExtensionProperties", &wine_vkEnumerateDeviceExtensionProperties},
    {"vkEnumerateDeviceLayerProperties", &wine_vkEnumerateDeviceLayerProperties},
    {"vkEnumeratePhysicalDevices", &wine_vkEnumeratePhysicalDevices},
    {"vkGetPhysicalDeviceFeatures", &wine_vkGetPhysicalDeviceFeatures},
    {"vkGetPhysicalDeviceFormatProperties", &wine_vkGetPhysicalDeviceFormatProperties},
    {"vkGetPhysicalDeviceImageFormatProperties", &wine_vkGetPhysicalDeviceImageFormatProperties},
    {"vkGetPhysicalDeviceMemoryProperties", &wine_vkGetPhysicalDeviceMemoryProperties},
    {"vkGetPhysicalDeviceProperties", &wine_vkGetPhysicalDeviceProperties},
    {"vkGetPhysicalDeviceQueueFamilyProperties", &wine_vkGetPhysicalDeviceQueueFamilyProperties},
    {"vkGetPhysicalDeviceSparseImageFormatProperties", &wine_vkGetPhysicalDeviceSparseImageFormatProperties},
};

void *wine_vk_get_instance_proc_addr(const char *name)
{
    int i;
    for (i = 0; i < sizeof(vk_instance_dispatch_table) / sizeof(vk_instance_dispatch_table[0]); i++)
    {
        if (strcmp(name, vk_instance_dispatch_table[i].name) == 0)
        {
            TRACE("Found pName=%s in instance table\n", name);
            return vk_instance_dispatch_table[i].func;
        }
    }
    return NULL;
}
