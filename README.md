This project is my Wine development repository in which I prepare my Vulkan patches.
It is meant for showing the development work without blasting wine-devel with huge
patchsets.

You can use this repo to play around with Vulkan on Wine, but this repo is not meant
to be stable. It will be rebased frequently, so be warned!

In order to use wine-vulkan:
1. Compile Wine as usual.
2. Then download the Vulkan SDK for the Vulkan loader: https://www.lunarg.com/vulkan-sdk/
3. Create a json file "c:\\windows\\system32\\winevulkan.json" containing:
{
    "file_format_version": "1.0.0",
    "ICD": {
        "library_path": "c:\\windows\\system32\\winevulkan.dll",
        "api_version": "1.0.51"
    }
}
4. Add a registry key:
[HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\Vulkan\Drivers\]
"C:\Windows\System32\winevulkan.json"=dword:00000000

At the moment the patchset provides Vulkan 1.0.51 core with minimal extensions for graphics
rendering. For licensing reasons vk.xml (to be resolved soon), it is not supporting newer
versions. The code is enough to run some basic Vulkan test applications on 64-bit
Wine including 'cube.exe', 'vulkaninfo.exe' and 'VkQuake'. Some other applications work
as well.

