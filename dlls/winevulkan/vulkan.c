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

#include "wine/debug.h"
#include "wine/vulkan.h"

WINE_DEFAULT_DEBUG_CHANNEL(vulkan);

/* For now default to 3 as it felt like a reasonable version feature wise to support.
 * Version 4 requires us to implement vk_icdGetPhysicalDeviceProcAddr, which I didn't
 * want to deal with just yet. It also added some more detailed API version check
 * requirements. Version 5 builds further on this. Something to tackle later.
 */
#define WINE_VULKAN_ICD_VERSION 3

void * WINAPI wine_vk_icdGetInstanceProcAddr(VkInstance instance, const char *pName)
{
    FIXME("stub: %p %s\n", instance, debugstr_a(pName));
    return NULL;
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
            DisableThreadLibraryCalls(hinst);
            break;

        case DLL_THREAD_ATTACH:
            break;
    }
    return TRUE;
}
