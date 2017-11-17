Wine-vulkan is my personal development branch for Wine Vulkan development. Originally
it was created as a staging area before my Vulkan work went into Wine. As of Wine 3.4
the majority of the work is integrated into Wine and major games such as Doom and
Wolfenstein II just work fine.

For most users there is little need to use wine-vulkan. I may continue updating this
project and add experimental patches, but in general I'm trying to get patches into
regular Wine as soon as possible.

You can use this repo to play around with Vulkan on Wine, but this repo is not meant
to be stable. It will be rebased frequently, so be warned!

Minimal instructions for using wine-vulkan:
1. Install 32-bit / 64-bit Vulkan libraries and drivers for your Linux distribution.
2. Compile Wine as usual.
3. You should now be able to run your Vulkan software.

The default wine-vulkan installation provides a minimal vulkan-1.dll, which is good for
most purposes and provides all Vulkan functionality. It lacks features such as support for
layers; a feature commonly used for debugging purposes, but some applications like Steam
can use it for an overlay. To use these features, the official Vulkan loader for Windows
can be used. If you don't need these features, there is no benefit of using the official
loader. In the contrary actually, the builtin vulkan-1.dll has less overhead and should
thus be slightly faster.

Instructions for using official Vulkan Windows loader:
1. Download and install the Windows Vulkan SDK for the Vulkan loader: https://www.lunarg.com/vulkan-sdk/
   Alternatively it can be installed through "winetricks vulkansdk", though needs a winetricks version from 3/10/18 or newer.
2. Copy winevulkan.json to "c:\\windows" in your wineprefix.
   Alternatively create a json file "c:\\windows\\winevulkan.json" containing:
```
{
    "file_format_version": "1.0.0",
    "ICD": {
        "library_path": "c:\\windows\\system32\\winevulkan.dll",
        "api_version": "1.0.51"
    }
}
```
3. Add registry key(s):
```
[HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\Vulkan\Drivers\]
"C:\Windows\winevulkan.json"=dword:00000000
```

If on 64-bit also add a line to load the json file for 32-bit:
```
[HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Khronos\Vulkan\Drivers\]
"C:\Windows\winevulkan.json"=dword:00000000
```

At the moment the patchset provides Vulkan 1.0.51 core with minimal extensions for graphics
rendering. For licensing reasons vk.xml (to be resolved soon), it is not supporting newer
versions. The code is enough to run some basic Vulkan test applications on 64-bit
Wine including 'The Talos Principle', 'Doom', 'Wolfenstein II', 'cube.exe', 'vulkaninfo.exe' and 'VkQuake'.
