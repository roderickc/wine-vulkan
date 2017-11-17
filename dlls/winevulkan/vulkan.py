#!/usr/bin/python
# Wine Vulkan generator
#
# Copyright 2017 Roderick Colenbrander
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
#

import argparse
import logging
import re
import sys
import xml.etree.ElementTree as ET
from collections import OrderedDict

LOGGER = logging.Logger("vulkan")
LOGGER.addHandler(logging.StreamHandler())

# Extension enum values start at a certain offset (EXT_BASE).
# Relative to the offset each extension has a block (EXT_BLOCK_SIZE)
# of values.
# Start for a given extension is:
# EXT_BASE + (extension_number-1) * EXT_BLOCK_SIZE
EXT_BASE = 1000000000
EXT_BLOCK_SIZE = 1000

BLACKLISTED_EXTENSIONS = [
    "VK_EXT_acquire_xlib_display",
    # Do we need to provide VK_EXT_debug_report? It seems to be getting
    # injected by the loader, which handles at least some of the implementation.
    # Do drivers actually provide implementations? At least Mesa ICDs don't provide
    # them. If we have to support this extension we need to wrap the callbacks.
    "VK_EXT_debug_report",
    "VK_EXT_display_control", # requires VK_EXT_display_surface_counter
    "VK_KHR_display", # Todo: add later. Nvidia supports this, Mesa drivers not yet.
    "VK_KHR_android_surface",
    "VK_KHR_xcb_surface",
    "VK_KHR_xlib_surface",
    "VK_KHR_mir_surface",
    "VK_KHR_wayland_surface",
    "VK_KHR_external_fence_fd",
    "VK_KHX_external_fence_fd",
    "VK_KHR_external_fence_win32",
    "VK_KHX_external_fence_win32",
    "VK_KHR_external_memory",
    "VK_KHX_external_memory_fd",
    "VK_KHX_external_memory_win32",
    "VK_KHR_external_semaphore",
    # Relates to external_semaphore and needs type conversions in bitflags.
    "VK_KHR_external_semaphore_capabilities",
    "VK_KHX_external_semaphore_capabilities",
    "VK_MVK_ios_surface",
    "VK_MVK_macos_surface",
    "VK_NN_vi_surface",
    "VK_NV_external_memory_win32"
]

# Functions part of our winevulkan graphics driver interface.
# DRIVER_VERSION should be bumped on any change to DRIVER_FUNCTIONS.
DRIVER_VERSION = 3
DRIVER_FUNCTIONS = [
    "vkCreateInstance",
    "vkCreateWin32SurfaceKHR",
    "vkDestroyInstance",
    "vkDestroySurfaceKHR",
    "vkEnumerateInstanceExtensionProperties",
    "vkGetDeviceProcAddr",
    "vkGetInstanceProcAddr",
    "vkGetPhysicalDeviceWin32PresentationSupportKHR"
]

# thunk sets whether a thunk needs to be created,
# dispatch whether we need a function pointer in
# the device / instance dispatch table
FUNCTION_OVERRIDES = {
    "vkGetDeviceProcAddr" : {"dispatch" : True, "thunk" : False },
    "vkAllocateCommandBuffers" : {"dispatch" : True, "thunk" : False },
    "vkCmdExecuteCommands" : {"dispatch" : True, "thunk" : False },
    "vkCreateDevice" : {"dispatch" : True, "thunk" : False },
    "vkCreateWin32SurfaceKHR" : {"dispatch" : True, "thunk" : False },
    "vkDestroyDevice" : {"dispatch" : True, "thunk" : False },
    "vkDestroyInstance" : {"dispatch" : True, "thunk" : False },
    "vkDestroySurfaceKHR" : {"dispatch" : True, "thunk" : False },
     # vkEnumerateInstanceLayerProperties doesn't make sense for an ICD since ICDs don't
     # support loaders and in addition the loader can't even call it due to lack of a
     # dispatchable first paramter.
    "vkEnumerateInstanceLayerProperties" : {"dispatch" : False, "thunk" : False},
    "vkEnumeratePhysicalDevices" : {"dispatch" : True, "thunk" : False },
    "vkFreeCommandBuffers" : {"dispatch" : True, "thunk" : False },
    "vkGetDeviceQueue" : {"dispatch": True, "thunk" : False },
    "vkGetPhysicalDeviceWin32PresentationSupportKHR" : {"dispatch" : True, "thunk" : False},
    "vkQueueSubmit" : {"dispatch": True, "thunk" : False },
}

# Filenames to create.
WINE_VULKAN_H = "../../include/wine/vulkan.h"
WINE_VULKAN_DRIVER_H = "../../include/wine/vulkan_driver.h"
WINE_VULKAN_THUNKS_C = "vulkan_thunks.c"
WINE_VULKAN_THUNKS_H = "vulkan_thunks.h"

class VkBaseType(object):
    def __init__(self, name, _type, requires=None):
        """ Vulkan base type class.

        VkBaseType is mostly used by Vulkan to define its own
        base types like VkFlags through typedef out of e.g. uint32_t.

        Args:
            name (:obj:'str'): Name of the base type.
            _type (:obj:'str'): Underlaying type
            requires (:obj:'str', optional): Other types required.
                Often bitmask values pull in a *FlagBits type.
        """
        self.name = name
        self.type = _type
        self.requires = requires
        self.required = False

    def definition(self):
        text = "typedef {0} {1};\n".format(self.type, self.name)
        return text


class VkConstant(object):
    def __init__(self, name, value):
        self.name = name
        self.value = value

    def definition(self):
        text = "#define {0} {1}\n".format(self.name, self.value)
        return text


class VkDefine(object):
    def __init__(self, name, value):
        self.name = name
        self.value = value

    @staticmethod
    def from_xml(define):
        name_elem = define.find("name")

        if name_elem is None:
            # <type category="define" name="some_name">some_value</type>
            # At the time of writing there is only 1 define of this category
            # 'VK_DEFINE_NON_DISPATCHABLE_HANDLE'.
            name = define.attrib.get("name")

            # We override behavior of VK_DEFINE_NON_DISPATCHABLE handle as the default
            # definition various between 64-bit (uses pointers) and 32-bit (uses uint64_t).
            # This complicates TRACEs in the thunks, so just use uint64_t.
            if name == "VK_DEFINE_NON_DISPATCHABLE_HANDLE":
                value = "#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef uint64_t object;"
            else:
                value = define.text
        else:
            # <type category="define"><name>some_name</name>some_value</type>
            name_elem = define.find("name")
            name = name_elem.text

            # This form of define laid out in a C-like form. Just grab the bits
            # and pieces and build the value. Due to amount of variation in
            # defines it doesn't make sense to parse this out in more detail.
            value = define.text
            for child in define:
                value += child.text
                if child.tail is not None:
                    value += child.tail

        return VkDefine(name, value)

    def definition(self):
        # Nothing to do as the value was already put in the right form during parsing.
        return "{0}\n".format(self.value)


class VkEnum(object):
    def __init__(self, name, values):
        self.name = name
        self.values = values
        self.required = False

    def add(self, value):
        """ Add a value to enum. """
        self.values.append(value)

    def definition(self):
        text = "typedef enum {0} {{\n".format(self.name)

        # Print values sorted, values can have been added in a random order.
        values = sorted(self.values, key=lambda value: value.value)
        for value in values:
            text += "    {0},\n".format(value.definition())
        text += "}} {0};\n\n".format(self.name)
        return text


class VkEnumValue(object):
    def __init__(self, name, value, hex=False):
        self.name = name
        self.value = value
        self.hex = hex

    def __repr__(self):
        return "{0}={1}".format(self.name, self.value)

    def definition(self):
        """ Convert to text definition e.g. VK_FOO = 1 """

        # Hex is commonly used for FlagBits and sometimes within
        # a non-FlagBits enum for a bitmask value as well.
        if self.hex:
            return "{0} = 0x{1:08X}".format(self.name, self.value)
        else:
            return "{0} = {1}".format(self.name, self.value)


class VkFunction(object):
    def __init__(self, _type=None, name=None, params=[], extension=None):
        self.extension = extension
        self.name = name
        self.type = _type
        self.params = params

        # Required is set while parsing which APIs and types are required
        # and is used by the code generation.
        self.required = False

    def is_device_func(self):
        # If none of the other, it must be a device function
        return not self.is_global_func() and not self.is_instance_func()

    def is_global_func(self):
        # Treat vkGetInstanceProcAddr as a global function as it
        # can operate with NULL for vkInstance.
        if self.name == "vkGetInstanceProcAddr":
            return True
        # Global functions are not passed a dispatchable object.
        elif self.params[0].is_dispatchable():
            return False
        return True

    def is_instance_func(self):
        # Instance functions are passed VkInstance or VkPhysicalDevice.
        if self.params[0].type in ["VkInstance", "VkPhysicalDevice"]:
            return True
        return False

    def needs_conversion(self):
        """ Check if the function needs any input/output type conversion.
        Funcsions need input/output conversion if struct parameters have
        alignment differences between Win32 and Linux 32-bit.
        """

        for p in self.params:
            if p.needs_conversion():
                LOGGER.debug("Parameter {0} to {1} requires conversion".format(p.name, self.name))
                return True

        return False

    def pfn(self, call_conv=None):
        """ Create function pointer. """

        if call_conv is not None:
            pfn = "{0} ({1} *p_{2})(".format(self.type, call_conv, self.name)
        else:
            pfn = "{0} (*p_{1})(".format(self.type, self.name)

        for i, param in enumerate(self.params):
            if param.const:
                pfn += param.const + " "

            pfn += param.type

            if param.pointer is not None:
                pfn += " " + param.pointer

            if param.array_len is not None:
                pfn += "[{0}]".format(param.array_len)

            if i < len(self.params) - 1:
                pfn += ", "
        pfn += ")"
        return pfn

    def proto(self, call_conv=None, prefix=None, postfix=None):
        """ Generate prototype for given function.

        Args:
            call_conv (str, optional): calling convention e.g. WINAPI
            prefix (str, optional): prefix to append prior to function name e.g. vkFoo -> wine_vkFoo
            postfix (str, optional): text to append after function name but prior to semicolon e.g. DECLSPEC_HIDDEN
        """

        proto = "{0:10}".format(self.type)

        if call_conv is not None:
            proto += " {0}".format(call_conv)

        if prefix is not None:
            proto += " {0}{1}(".format(prefix, self.name)
        else:
            proto += " {0}(".format(self.name)

        first = True
        for param in self.params:
            if first:
                first = False
            else:
                proto += ", "

            if param.const:
                proto += param.const + " "

            proto += param.type

            if param.pointer:
                proto += " {0}{1}".format(param.pointer, param.name)
            else:
                proto += " " + param.name

            if param.array_len is not None:
                proto += "[{0}]".format(param.array_len)

        if postfix is not None:
            proto += ") {0};".format(postfix)
        else:
            proto += ");"

        return proto

    def thunk(self, call_conv=None, prefix=None):
        thunk = self.type

        if call_conv is not None:
            thunk += " " + call_conv

        if prefix is not None:
             thunk += " {0}{1}(".format(prefix, self.name)
        else:
             thunk += " {0}(".format(self.name)

        first = True
        for param in self.params:
            if first:
                thunk += "{0}".format(param.definition())
                first = False
            else:
                thunk += ", {0}".format(param.definition())
        thunk += ")\n{\n"

        thunk += "    " + self.trace()

        if self.type != "void":
            thunk += "    return "
        else:
            thunk += "    "

        params = ", ".join([p.code() for p in self.params])
        thunk += "{0}.p_{1}({2});\n".format(self.params[0].dispatch_table(), self.name, params)
        thunk += "}\n\n"
        return thunk

    def trace(self):
        trace = "TRACE("

        # First loop is for all the format strings.
        first = True
        for param in self.params:
            if first:
                trace += "\"{0}".format(param.format_string())
                first = False
            else:
                trace += ", {0}".format(param.format_string())
        trace += "\\n\""

        # Second loop for parameter names and optional conversions.
        for param in self.params:
            if param.format_conv is not None:
                trace += ", " + param.format_conv.format(param.name)
            else:
                trace += ", {0}".format(param.name)
        trace += ");\n"

        return trace


class VkFunctionPointer(object):
    def __init__(self, _type, name, members):
        self.name = name
        self.members = members
        self.type = _type
        self.required = False

    @staticmethod
    def from_xml(funcpointer):
        members = []
        begin = None

        for t in funcpointer.findall("type"):
            # General form:
            # <type>void</type>*       pUserData,
            # Parsing of the tail (anything past </type>) is tricky since there
            # can be other data on the next line like: const <type>int</type>..
            const = begin
            _type = t.text
            lines = t.tail.split(",\n")
            if lines[0][0] == "*":
                pointer = "*"
                name = lines[0][1:].strip()
            else:
                pointer = None
                name = lines[0].strip()

            # Filter out ); if it is contained.
            name = name.partition(");")[0]

            # If tail encompasses multiple lines, assign the second line to begin
            # for the next line.
            try:
                begin = lines[1].strip()
            except IndexError:
                begin = None

            members.append(VkMember(const=const, _type=_type, pointer=pointer, name=name))

        _type = funcpointer.text
        name = funcpointer.find("name").text
        return VkFunctionPointer(_type, name, members)

    def definition(self):
        text = "{0} {1})(\n".format(self.type, self.name)

        first = True
        if len(self.members) > 0:
            for m in self.members:
                if first:
                    text += "    " + m.definition()
                    first = False
                else:
                    text += ",\n    " + m.definition()
        else:
            # Just make the compiler happy by adding a void parameter.
            text += "void"
        text += ");\n"
        return text


class VkHandle(object):
    def __init__(self, name, _type, parent):
        self.name = name
        self.type = _type
        self.parent = parent
        self.required = False

    def is_dispatchable(self):
        """ Some handles like VkInstance, VkDevice are dispatchable objects,
        which means they contain a dispatch table of function pointers.
        """
        return self.type == "VK_DEFINE_HANDLE"

    def dispatch_table(self):
        if not self.is_dispatchable():
            return None

        if self.parent is None:
            # Should only happen for VkInstance
            return "funcs"
        elif self.name == "VkDevice":
            # VkDevice has VkInstance as a parent, but has its own dispatch table.
            return "funcs"
        elif self.parent in ["VkInstance", "VkPhysicalDevice"]:
            return "instance->funcs"
        elif self.parent in ["VkDevice", "VkCommandPool"]:
            return "device->funcs"
        else:
            LOGGER.error("Unhandled dispatchable parent: {0}".format(self.parent))

    def definition(self):
        # Generates handle definition e.g. VK_DEFINE_HANDLE(vkInstance)
        text = "{0}({1})\n".format(self.type, self.name)
        return text

    def native_handle(self):
        """ Provide access to the native handle of a dispatchable object.

        Dispatchable objects wrap an underlaying 'native' object.
        This method provides access to the native object.
        """
        if not self.is_dispatchable():
            return None

        if self.name == "VkCommandBuffer":
            return "command_buffer"
        elif self.name == "VkDevice":
            return "device"
        elif self.name == "VkInstance":
            return "instance"
        elif self.name == "VkPhysicalDevice":
            return "phys_dev"
        elif self.name == "VkQueue":
            return "queue"
        else:
            LOGGER.error("Unhandled native handle for: {0}".format(self.name))


class VkMember(object):
    def __init__(self, const=None, _type=None, pointer=None, name=None, array_len=None):
        self.const = const
        self.name = name
        self.pointer = pointer
        self.type = _type
        self.type_info = None
        self.array_len = array_len

    def __repr__(self):
        return "{0} {1} {2} {3} {4}".format(self.const, self.type, self.pointer, self.name, self.array_len)

    @staticmethod
    def from_xml(member):
        """ Helper function for parsing a member tag within a struct or union. """

        name_elem = member.find("name")
        type_elem = member.find("type")

        const = member.text.strip() if member.text else None
        member_type = None
        pointer = None
        array_len = None

        if type_elem is not None:
            member_type = type_elem.text
            if type_elem.tail is not None:
                pointer = type_elem.tail.strip() if type_elem.tail.strip() != "" else None

        # Some members are arrays, attempt to parse these. Formats include:
        # <member><type>char</type><name>extensionName</name>[<enum>VK_MAX_EXTENSION_NAME_SIZE</enum>]</member>
        # <member><type>uint32_t</type><name>foo</name>[4]</member>
        if name_elem.tail and name_elem.tail[0] == '[':
            LOGGER.debug("Found array type")
            enum_elem = member.find("enum")
            if enum_elem is not None:
                array_len = enum_elem.text
            else:
                # Remove brackets around length
                array_len = name_elem.tail.strip("[]")

        return VkMember(const=const, _type=member_type, pointer=pointer, name=name_elem.text, array_len=array_len)

    def definition(self, align=False, postfix=None):
        text = ""
        if self.const is not None:
            text += "const "

        if postfix is not None:
            text += "{0}{1}".format(self.type, postfix)
        else:
            text += self.type

        if self.pointer is not None:
            text += " {0}{1}".format(self.pointer, self.name)
        else:
            if align and self.needs_alignment():
                text += " WINE_VK_ALIGN(8) " + self.name
            else:
                text += " " + self.name

        if self.array_len is not None:
            text += "[{0}]".format(self.array_len)

        return text

    def is_struct(self):
        return self.type_info["category"] == "struct"

    def needs_alignment(self):
        """ Check if this member needs alignment for 64-bit data.
        Various structures need alignment on 64-bit variables due
        to compiler differences on 32-bit between Win32 and Linux.
        """

#todo array!!
        if self.pointer is not None:
            return False
        elif self.type == "size_t":
            return False
        elif self.type in ["uint64_t", "VkDeviceSize"]:
            return True
        elif self.type_info["category"] == "struct":
            struct = self.type_info["data"]
            return struct.needs_alignment()
        elif self.type_info["category"] == "handle":
            # Dispatchable handles are pointers to objects, while
            # non-dispatchable are uint64_t and hence need alignment.
            handle = self.type_info["data"]
            return False if handle.is_dispatchable() else True
        return False

    def set_type_info(self, type_info):
        """ Helper function to set type information from the type registry.
        This is needed, because not all type data is available at time of
        parsing.
        """
        self.type_info = type_info


class VkParam(object):
    """ Helper class which describes a parameter to a function call. """

    def __init__(self, type_info, const=None, pointer=None, name=None, array_len=None):
        self.const = const
        self.name = name
        self.array_len = array_len
        self.pointer = pointer
        self.type_info = type_info
        self.type = type_info["name"] # Just for convenience
        self.handle = type_info["data"] if type_info["category"] == "handle" else None

        # Determine a format string used by code generation for traces.
        # 64-bit types need a conversion function.
        self.format_conv = None
        if array_len is not None or pointer is not None:
            self.format_str = "%p"
        else:
            if type_info["category"] == "bitmask":
                self.format_str = "%#x"
            elif type_info["category"] == "enum":
                self.format_str = "%d"
            elif type_info["category"] == "handle":
                # We use uint64_t for non-dispatchable handles as opposed to pointers
                # for dispatchable handles.
                handle = type_info["data"]
                if handle.is_dispatchable():
                    self.format_str = "%p"
                else:
                    self.format_str = "0x%s"
                    self.format_conv = "wine_dbgstr_longlong({0})"
            elif type_info["name"] == "float":
                self.format_str = "%f"
            elif type_info["name"] == "int":
                self.format_str = "%d"
            elif type_info["name"] == "int32_t":
                self.format_str = "%d"
            elif type_info["name"] == "size_t":
                self.format_str = "0x%s"
                self.format_conv = "wine_dbgstr_longlong({0})"
            elif type_info["name"] in ["uint32_t", "VkBool32"]:
                self.format_str = "%u"
            elif type_info["name"] in ["uint64_t", "VkDeviceSize"]:
                self.format_str = "0x%s"
                self.format_conv = "wine_dbgstr_longlong({0})"
            else:
                LOGGER.warn("Unhandled type: {0}".format(type_info))

    def __repr__(self):
        return "{0} {1} {2} {3} {4}".format(self.const, self.type, self.pointer, self.name, self.array_len)

    @staticmethod
    def from_xml(param, types):
        """ Helper function to create VkParam from xml. """

        # Parameter parsing is slightly tricky. All the data is contained within
        # a param tag, but some data is within subtags while others are text
        # before or after the type tag.
        # Common structure:
        # <param>const <type>char</type>* <name>pLayerName</name></param>

        name_elem = param.find("name")
        array_len = None
        name = name_elem.text
        # Tail contains array length e.g. for blendConstants param of vkSetBlendConstants
        if name_elem.tail is not None:
            array_len = name_elem.tail.strip("[]")

        const = param.text.strip() if param.text else None
        type_elem = param.find("type")
        pointer = type_elem.tail.strip() if type_elem.tail.strip() != "" else None

        # Since we have parsed all types before hand, this should not happen.
        type_info = types.get(type_elem.text, None)
        if type_info is None:
            LOGGER.err("type info not found for: {0}".format(type_elem.text))

        return VkParam(type_info, const=const, pointer=pointer, name=name, array_len=array_len)

    def is_dispatchable(self):
        if self.handle is None:
            return False

        return self.handle.is_dispatchable()

    def dispatch_table(self):
        """ Return functions dispatch table pointer for dispatchable objects. """

        if not self.is_dispatchable():
            return None

        return "{0}->{1}".format(self.name, self.handle.dispatch_table())

    def needs_conversion(self):
        """ Returns if parameter needs conversion between win32 and host. """

        if not self.type_info["category"] == "struct":
            return False

        # If a structure needs alignment changes, it means we need to
        # perform parameter conversion between win32 and host.
        struct = self.type_info["data"]
        if struct.needs_alignment():
            return True

        return False

    def code(self):
        # Hack until we enabled callbacks.
        if "VkAllocationCallbacks" in self.type:
            LOGGER.debug("HACK: setting NULL VkAllocationCallbacks for {0}".format(self.name))
            return "NULL"

        # Dispatchable objects wrap the native handle. For thunk generation we
        # need to pass the native handle to the native vulkan calls.
        if self.is_dispatchable():
            return "{0}->{1}".format(self.name, self.handle.native_handle())
        else:
            return self.name

    def definition(self, postfix=None):
        """ Return prototype for the parameter. E.g. 'const char *foo' """

        proto = ""
        if self.const:
            proto += self.const + " "

        proto += self.type

        if self.pointer is not None:
            proto += " {0}{1}".format(self.pointer, self.name)
        else:
            proto += " " + self.name

        # Allows appeninding something to the variable name useful for
        # win32 to host conversion.
        if postfix is not None:
            proto += postfix

        if self.array_len is not None:
            proto += "[{0}]".format(self.array_len)

        return proto

    def format_string(self):
        return self.format_str


class VkStruct(object):
    """ Class which represents the type union and struct. """

    def __init__(self, name, members, returnedonly, union=False):
        self.name = name
        self.members = members
        self.returnedonly = returnedonly
        self.required = False
        self.union = union
        self.type_info = None # To be set later.

    @staticmethod
    def from_xml(struct):
        # Unions and structs are the same parsing wise, but we need to
        # know which one we are dealing with later on for code generation.
        union = True if struct.attrib["category"] == "union" else False

        name = struct.attrib.get("name", None)

        # 'Output' structures for which data is filled in by the API are
        # marked as 'returnedonly'.
        returnedonly = True if struct.attrib.get("returnedonly") else False

        members = []
        for member in struct.findall("member"):
            vk_member = VkMember.from_xml(member)
            members.append(vk_member)

        return VkStruct(name, members, returnedonly, union=union)

    def definition(self, align=False, postfix=None):
        """ Convert structure to textual definition.

        Args:
            align (bool, optional): enable alignment to 64-bit for win32 struct compatibility.
            postfix (str, optional): text to append to end of struct name, useful for struct renaming.
        """
        if self.union:
            if postfix is not None:
                text = "typedef union {0}{1}\n{{\n".format(self.name, postfix)
            else:
                text = "typedef union {0}\n{{\n".format(self.name)
        else:
            if postfix is not None:
                text = "typedef struct {0}{1}\n{{\n".format(self.name, postfix)
            else:
                text = "typedef struct {0}\n{{\n".format(self.name)

        for m in self.members:
            if align and m.needs_alignment():
                text += "    {0};\n".format(m.definition(align=align, postfix=postfix))
            else:
                text += "    {0};\n".format(m.definition())

        if postfix is not None:
            text += "}} {0}{1};\n\n".format(self.name, postfix)
        else:
            text += "}} {0};\n\n".format(self.name)
        return text

    def needs_alignment(self):
        """ Check if structure needs alignment for 64-bit data.
        Various structures need alignment on 64-bit variables due
        to compiler differences on 32-bit between Win32 and Linux.
        """
        for m in self.members:
            if m.needs_alignment():
                return True
        return False

    def set_type_info(self, types):
        """ Helper function to set type information from the type registry.
        This is needed, because not all type data is available at time of
        parsing.
        """
        for m in self.members:
            type_info = types[m.type]
            m.set_type_info(type_info)


class VkGenerator(object):
    def __init__(self, registry):
        self.registry = registry

    def generate_thunks_c(self, f, prefix):
        f.write("/* Automatically generated from Vulkan vk.xml; DO NOT EDIT! */\n\n")

        f.write("#include \"config.h\"\n")
        f.write("#include \"wine/port.h\"\n\n")

        f.write("#include \"wine/debug.h\"\n")
        f.write("#include \"wine/vulkan.h\"\n")
        f.write("#include \"wine/vulkan_driver.h\"\n")
        f.write("#include \"vulkan_private.h\"\n\n")

        f.write("WINE_DEFAULT_DEBUG_CHANNEL(vulkan);\n\n")

        # Create thunks for instance and device functions.
        # Global functions don't go through the thunks.
        for vk_func in self.registry.funcs.values():
            if not vk_func.required:
                continue

            if vk_func.is_global_func():
                continue

            # We handle some functions in a special way
            func_info = FUNCTION_OVERRIDES.get(vk_func.name, None)
            if func_info and func_info["thunk"] == False:
                continue

            f.write("static " + vk_func.thunk(prefix=prefix, call_conv="WINAPI"))

        f.write("static const struct vulkan_func vk_device_dispatch_table[] = {\n")
        for vk_func in self.registry.device_funcs:
            if not vk_func.required:
                continue

            func_info = FUNCTION_OVERRIDES.get(vk_func.name, None)
            if func_info and func_info["dispatch"] == False:
                LOGGER.debug("skipping {0} in device dispatch table".format(vk_func.name))
                continue

            f.write("    {{\"{0}\", &{1}{0}}},\n".format(vk_func.name, prefix))
        f.write("};\n\n")

        f.write("static const struct vulkan_func vk_instance_dispatch_table[] = {\n")
        for vk_func in self.registry.instance_funcs:
            if not vk_func.required:
                continue

            func_info = FUNCTION_OVERRIDES.get(vk_func.name, None)
            if func_info and func_info["dispatch"] == False:
                LOGGER.debug("skipping {0} in instance dispatch table".format(vk_func.name))
                continue

            f.write("    {{\"{0}\", &{1}{0}}},\n".format(vk_func.name, prefix))
        f.write("};\n")

        f.write("void *wine_vk_get_device_proc_addr(const char *name)\n")
        f.write("{\n")
        f.write("    int i;\n")
        f.write("    for (i = 0; i < sizeof(vk_device_dispatch_table) / sizeof(vk_device_dispatch_table[0]); i++)\n")
        f.write("    {\n")
        f.write("        if (strcmp(name, vk_device_dispatch_table[i].name) == 0)\n")
        f.write("        {\n")
        f.write("            TRACE(\"Found pName=%s in device table\\n\", name);\n")
        f.write("            return vk_device_dispatch_table[i].func;\n")
        f.write("        }\n")
        f.write("    }\n")
        f.write("    return NULL;\n")
        f.write("}\n\n")

        f.write("void *wine_vk_get_instance_proc_addr(const char *name)\n")
        f.write("{\n")
        f.write("    int i;\n")
        f.write("    for (i = 0; i < sizeof(vk_instance_dispatch_table) / sizeof(vk_instance_dispatch_table[0]); i++)\n")
        f.write("    {\n")
        f.write("        if (strcmp(name, vk_instance_dispatch_table[i].name) == 0)\n")
        f.write("        {\n")
        f.write("            TRACE(\"Found pName=%s in instance table\\n\", name);\n")
        f.write("            return vk_instance_dispatch_table[i].func;\n")
        f.write("        }\n")
        f.write("    }\n")
        f.write("    return NULL;\n")
        f.write("}\n")

    def generate_thunks_h(self, f, prefix):
        f.write("/* Automatically generated from Vulkan vk.xml; DO NOT EDIT! */\n\n")

        f.write("#ifndef __WINE_VULKAN_THUNKS_H\n")
        f.write("#define __WINE_VULKAN_THUNKS_H\n\n")

        f.write("/* For use by vk_icdGetInstanceProcAddr / vkGetInstanceProcAddr */\n")
        f.write("void *wine_vk_get_device_proc_addr(const char *name) DECLSPEC_HIDDEN;\n")
        f.write("void *wine_vk_get_instance_proc_addr(const char *name) DECLSPEC_HIDDEN;\n\n")

        # Generate prototypes for device and instance functions requiring a custom implementation.
        f.write("/* Functions for which we have custom implementations outside of the thunks. */\n")
        for vk_func in self.registry.funcs.values():
            if vk_func.is_global_func():
                continue
            if not vk_func.name in FUNCTION_OVERRIDES:
                continue
            f.write("{0}\n".format(vk_func.proto("WINAPI", prefix="wine_", postfix="DECLSPEC_HIDDEN")))
        f.write("\n")

        f.write("/* For use by vkDevice and children */\n")
        f.write("struct vulkan_device_funcs\n{\n")
        for vk_func in self.registry.device_funcs:
            if not vk_func.required:
                continue

            func_info = FUNCTION_OVERRIDES.get(vk_func.name, None)
            if func_info and func_info["dispatch"] == False:
                LOGGER.debug("skipping {0} in vulkan_device_funcs".format(vk_func.name))
                continue

            f.write("    {0};\n".format(vk_func.pfn()))
        f.write("};\n\n")

        f.write("/* For use by vkInstance and children */\n")
        f.write("struct vulkan_instance_funcs\n{\n")
        for vk_func in self.registry.instance_funcs:
            if not vk_func.required:
                continue

            func_info = FUNCTION_OVERRIDES.get(vk_func.name, None)
            if func_info and func_info["dispatch"] == False:
                LOGGER.debug("skipping {0} in vulkan_instance_funcs".format(vk_func.name))
                continue

            f.write("    {0};\n".format(vk_func.pfn()))
        f.write("};\n\n")

        f.write("#define ALL_VK_DEVICE_FUNCS() \\\n")
        first = True
        for vk_func in self.registry.device_funcs:
            if not vk_func.required:
                continue

            func_info = FUNCTION_OVERRIDES.get(vk_func.name, None)
            if func_info and func_info["dispatch"] == False:
                LOGGER.debug("skipping {0} in ALL_VK_DEVICE_FUNCS".format(vk_func.name))
                continue

            if first:
                f.write("    USE_VK_FUNC({0})".format(vk_func.name))
                first = False
            else:
                f.write(" \\\n    USE_VK_FUNC({0})".format(vk_func.name))
        f.write("\n\n")

        f.write("#define ALL_VK_INSTANCE_FUNCS() \\\n")
        first = True
        for vk_func in self.registry.instance_funcs:
            if not vk_func.required:
                continue

            func_info = FUNCTION_OVERRIDES.get(vk_func.name, None)
            if func_info and func_info["dispatch"] == False:
                LOGGER.debug("skipping {0} in ALL_VK_INSTANCE_FUNCS".format(vk_func.name))
                continue

            if first:
                f.write("    USE_VK_FUNC({0})".format(vk_func.name))
                first = False
            else:
                f.write("\\\n    USE_VK_FUNC({0})".format(vk_func.name))
        f.write("\n\n")

        f.write("#endif /* __WINE_VULKAN_THUNKS_H */\n")

    def generate_vulkan_h(self, f):
        f.write("/* Automatically generated from Vulkan vk.xml; DO NOT EDIT! */\n\n")
        f.write("#ifndef __WINE_VULKAN_H\n")
        f.write("#define __WINE_VULKAN_H\n\n")

        f.write("#include <windef.h>\n")
        f.write("#include <stdint.h>\n\n")

        f.write("#ifndef VKAPI_CALL\n")
        f.write("#define VKAPI_CALL __stdcall\n")
        f.write("#endif\n\n")

        f.write("#ifndef VKAPI_PTR\n")
        f.write("#define VKAPI_PTR VKAPI_CALL\n")
        f.write("#endif\n\n")

        f.write("/* Callers can override WINE_VK_ALIGN if they want 'host' headers. */\n")
        f.write("#ifndef WINE_VK_ALIGN\n")
        f.write("#define WINE_VK_ALIGN DECLSPEC_ALIGN\n")
        f.write("#endif\n\n")

        # The overall strategy is to define independent constants and datatypes,
        # prior to complex structures and function calls to avoid forward declarations.
        for const in self.registry.consts:
            # For now just generate things we may not need. The amount of parsing needed
            # to get some of the info is tricky as you need to figure out which structure
            # references a certain constant.
            f.write(const.definition())
        f.write("\n")

        for define in self.registry.defines:
            f.write(define.definition())

        for handle in self.registry.handles:
            if handle.required:
                 f.write(handle.definition())
        f.write("\n")

        for base_type in self.registry.base_types:
            f.write(base_type.definition())
        f.write("\n")

        for bitmask in self.registry.bitmasks:
            f.write(bitmask.definition())
        f.write("\n")

        # Define enums, this includes values for some of the bitmask types as well.
        for enum in self.registry.enums.values():
            if enum.required:
                f.write(enum.definition())

        for fp in self.registry.funcpointers:
            if fp.required:
                f.write(fp.definition())
        f.write("\n")

        # This generates both structures and unions. Since structures
        # may depend on other structures/unions, we loop multiple times.
        # Note: unions are stored in structs for dependency reasons,
        # see comment in parsing section.
        defined_structs = []
        structs = list(self.registry.structs)
        while (len(structs) > 0):
            for struct in structs:
                dependends = False

                if struct.required == False:
                    LOGGER.debug("Skipping struct: {0}".format(struct.name))
                    structs.remove(struct)
                    continue

                LOGGER.debug("Generating struct: {0}".format(struct.name))

                for m in struct.members:
                    type_info = self.registry.types[m.type]
                    if not type_info["category"] in ["struct", "union"]:
                        continue

                    # Check if a struct we depend on has already been defined.
                    for s in defined_structs:
                        if s.name == m.type:
                            break
                    else:
                        dependends = True

                if dependends == False:
                    # Write down the structure with alignment.
                    f.write(struct.definition(align=True))
                    defined_structs.append(struct)
                    structs.remove(struct)

        for func in self.registry.funcs.values():
            if not func.required:
                LOGGER.debug("Skipping API definition for: {0}".format(func.name))
                continue

            LOGGER.debug("Generating API definition for: {0}".format(func.name))
            f.write("{0}\n".format(func.proto(call_conv="VKAPI_CALL")))
        f.write("\n")

        f.write("#endif /* __WINE_VULKAN_H */\n")

    def generate_vulkan_driver_h(self, f):
        f.write("/* Automatically generated from Vulkan vk.xml; DO NOT EDIT! */\n\n")
        f.write("#ifndef __WINE_VULKAN_DRIVER_H\n")
        f.write("#define __WINE_VULKAN_DRIVER_H\n\n")

        f.write("/* Wine internal vulkan driver version, needs to be bumped upon vulkan_funcs changes. */\n")
        f.write("#define WINE_VULKAN_DRIVER_VERSION {0}\n\n".format(DRIVER_VERSION))

        f.write("struct vulkan_funcs\n{\n")
        f.write("    /* Vulkan global functions. This are the only calls at this point a graphics driver\n")
        f.write("     * needs to provide. Other function calls will be provided indirectly by dispatch\n")
        f.write("     * tables part of dispatchable Vulkan objects such as VkInstance or vkDevice.\n")
        f.write("     */\n")
        for func_name in DRIVER_FUNCTIONS:
            vk_func = self.registry.funcs[func_name]
            pfn = vk_func.pfn()
            # Avoid PFN_vkVoidFunction in driver interface as Vulkan likes to put calling convention
            # stuff in there. For simplicity substitute with "void *".
            pfn = pfn.replace("PFN_vkVoidFunction", "void *")
            f.write("    {0};\n".format(pfn))
        f.write("};\n\n")

        f.write("extern struct vulkan_funcs * CDECL __wine_get_vulkan_driver(HDC hdc, UINT version);\n\n")
        f.write("#endif /* __WINE_VULKAN_DRIVER_H */\n")


class VkRegistry(object):
    def __init__(self, reg_filename):
        # Used for storage of type information.
        self.base_types = None
        self.bitmasks = None
        self.consts = None
        self.defines = None
        self.enums = None
        self.funcpointers = None
        self.handles = None
        self.structs = None

        # We aggregate all types in here for cross-referencing.
        self.funcs = {}
        self.types = {}

        # Overall strategy for parsing the registry is to first
        # parse all type / function definitions. Then parse
        # features and extensions to decide which types / functions
        # to actually 'pull in' for code generation. For each type or
        # function call we want we set a member 'required' to True.
        tree = ET.parse(reg_filename)
        root = tree.getroot()
        self._parse_enums(root)
        self._parse_types(root)
        self._parse_commands(root)

        # Pull in any required types and functions.
        self._parse_features(root)
        self._parse_extensions(root)

    def _mark_command_required(self, command):
        def mark_bitmask_dependencies(bitmask, types):
            if bitmask.requires is not None:
                types[bitmask.requires]["data"].required = True

        def mark_funcpointer_dependencies(fp, types):
            for m in fp.members:
                type_info = types[m.type]

                # Complex types have a matching definition e.g. VkStruct.
                # Not needed for base types such as uint32_t.
                if "data" in type_info:
                    types[m.type]["data"].required = True

        def mark_struct_dependencies(struct, types):
             for m in struct.members:
                type_info = types[m.type]

                # Complex types have a matching definition e.g. VkStruct.
                # Not needed for base types such as uint32_t.
                if "data" in type_info:
                    types[m.type]["data"].required = True

                if type_info["category"] == "struct":
                    # Yay, recurse
                    mark_struct_dependencies(type_info["data"], types)
                elif type_info["category"] == "funcpointer":
                    mark_funcpointer_dependencies(type_info["data"], types)
                elif type_info["category"] == "bitmask":
                    mark_bitmask_dependencies(type_info["data"], types)

        func = self.funcs[command]
        func.required = True

        # Pull in return type
        if func.type != "void":
            self.types[func.type]["data"].required = True

        # Analyze parameter dependencies and pull in any type needed.
        for p in func.params:
            type_info = self.types[p.type]

            # Check if we are dealing with a complex type e.g. VkEnum, VkStruct and others.
            if "data" not in type_info:
                continue

            # Mark the complex type as required.
            type_info["data"].required = True
            if type_info["category"] == "struct":
                struct = type_info["data"]
                mark_struct_dependencies(struct, self.types)

    def _parse_commands(self, root):
        funcs = {}
        commands = root.findall("./commands/")
        for command in commands:
            proto = command.find("proto")

            func_name = proto.find("name").text
            func_type = proto.find("type").text

            params = []
            for param in command.findall("param"):
                vk_param = VkParam.from_xml(param, self.types)
                params.append(vk_param)

            func = VkFunction(_type=func_type, name=func_name, params=params)
            funcs[func_name] = func

        # To make life easy for the code generation, separate all function
        # calls out in the 3 types of vulkan functions: device, global and instance.
        device_funcs = []
        global_funcs = []
        instance_funcs = []
        for func in funcs.values():
            if func.is_device_func():
                device_funcs.append(func)
            elif func.is_global_func():
                global_funcs.append(func)
            else:
                instance_funcs.append(func)

        # Sort function lists by name and store them.
        self.device_funcs = sorted(device_funcs, key=lambda func: func.name)
        self.global_funcs = sorted(global_funcs, key=lambda func: func.name)
        self.instance_funcs = sorted(instance_funcs, key=lambda func: func.name)

        # The funcs dictionary is used as a convenient way to lookup function
        # calls when needed e.g. to adjust member variables.
        self.funcs = OrderedDict(sorted(funcs.items()))

    def _parse_enums(self, root):
        """ Parse enums section or better described as constants section. """
        enums = {}
        self.consts = []
        for enum in root.findall("./enums"):
            name = enum.attrib.get("name")
            _type = enum.attrib.get("type")

            if _type in ("enum", "bitmask"):
                values = []
                for v in enum.findall("enum"):
                    # Value is either a value or a bitpos, only one can exist.
                    value = v.attrib.get("value")
                    if value is None:
                        # bitmask
                        value = 1 << int(v.attrib.get("bitpos"))
                        values.append(VkEnumValue(v.attrib.get("name"), value, hex=True))
                    else:
                        # Some values are in hex form. We want to preserve the hex representation
                        # at least when we convert back to a string. Internally we want to use int.
                        if "0x" in value:
                            values.append(VkEnumValue(v.attrib.get("name"), int(value, 0), hex=True))
                        else:
                            values.append(VkEnumValue(v.attrib.get("name"), int(value, 0)))

                # vulkan.h contains a *_MAX_ENUM value set to 32-bit at the time of writing,
                # which is to prepare for extensions as they can add values and hence affect
                # the size definition.
                max_name = re.sub(r'([0-9a-z_])([A-Z0-9])',r'\1_\2',name).upper() + "_MAX_ENUM"
                values.append(VkEnumValue(max_name, 0x7fffffff, hex=True))

                enums[name] = VkEnum(name, values)
            else:
                # If no type is set, we are dealing with API constants.
                values = []
                for value in enum.findall("enum"):
                    self.consts.append(VkConstant(value.attrib.get("name"), value.attrib.get("value")))

        self.enums = OrderedDict(sorted(enums.items()))

    def _parse_extensions(self, root):
        exts = root.findall("./extensions/extension")
        for ext in exts:
            # Temporary whitelist of the minimal extensions we until the Vulkan core is right and we enable everything
            # except for blacklisted extensions.
            if not ext.attrib["name"] in ["VK_KHR_surface", "VK_KHR_win32_surface", "VK_KHR_swapchain"]:
                LOGGER.debug("Skipping: {0}".format(ext.attrib["name"]))
                continue

            # We disable some extensions as either we haven't implemented
            # support yet or because they are for platforms other than win32.
            if ext.attrib["name"] in BLACKLISTED_EXTENSIONS:
                continue
            elif "requires" in ext.attrib:
                # Check if this extension builds on top of another blacklisted
                # extension.
                requires = ext.attrib["requires"].split(",")
                if len(set(requires).intersection(BLACKLISTED_EXTENSIONS)) > 0:
                    continue

            enums = ext.findall("require/enum")
            for enum_elem in enums:
                if "bitpos" in enum_elem.keys():
                    # We need to add an extra value to an existing enum type.
                    # E.g. VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG to VkFormatFeatureFlagBits.
                    type_name = enum_elem.attrib["extends"]
                    enum = self.types[type_name]["data"]
                    enum.add(VkEnumValue(enum_elem.attrib["name"], 1 << int(enum_elem.attrib["bitpos"]), hex=True))
                elif "offset" in enum_elem.keys():
                    ext_number = int(ext.attrib["number"])
                    offset = int(enum_elem.attrib["offset"])
                    value = EXT_BASE + (ext_number - 1) * EXT_BLOCK_SIZE + offset

                    # Deal with negative values.
                    direction = enum_elem.attrib.get("dir")
                    if direction is not None:
                        value = -value

                    type_name = enum_elem.attrib["extends"]
                    enum = self.types[type_name]["data"]
                    enum.add(VkEnumValue(enum_elem.attrib["name"], value))

                elif "value" in enum_elem.keys():
                    # For now skip, it mostly contains extension name and version info.
                    continue
                else:
                    # This seems to be used to pull in constants e.g. VK_MAX_DEVICE_GROUP_KHX
                    continue

            commands = ext.findall("require/command")
            if not commands:
                continue

            # Pull in any commands we need. We infer types to pull in from the command
            # as well.
            for command in commands:
                name = command.attrib["name"]
                self._mark_command_required(name)

                # Set extension name on the function call as we were not aware of the
                # name during initial parsing.
                self.funcs[name].extension = ext.attrib["name"]

    def _parse_features(self, root):
        requires = root.findall("./feature/require")

        for require in requires:
            LOGGER.info("Including features for {0}".format(require.attrib.get("comment")))
            for tag in require:
                # Only deal with command. Other values which appear are enum and type for pulling in some
                # constants and macros. Tricky to parse, so don't bother right now, we will generate them
                # anyway for now.
                name = tag.attrib["name"]
                if tag.tag == "command":
                    self._mark_command_required(name)
                elif tag.tag == "enum":
                    # We could pull in relevant constants here. Unfortunately
                    # this only gets half of them pulled in as others indirectly
                    # get pulled in through structures. Constants don't harm us,
                    # so don't bother.
                    pass
                elif tag.tag == "type":
                    # Pull in types which may not have been pulled in through commands.

                    # Skip pull in for vk_platform.h for now.
                    if name == "vk_platform":
                        continue

                    type_info = self.types[name]
                    type_info["data"].required = True

    def _parse_types(self, root):
        types = root.findall("./types/type")

        base_types = []
        bitmasks = []
        defines = []
        funcpointers = []
        handles = []
        structs = []
        for t in types:
            type_info = {}
            type_info["category"] = t.attrib.get("category", None)

            if type_info["category"] in ["include"]:
                continue

            if type_info["category"] == "basetype":
                name = t.find("name").text
                _type = t.find("type").text
                basetype = VkBaseType(name, _type)
                base_types.append(basetype)
                type_info["data"] = basetype

            if type_info["category"] == "bitmask":
                name = t.find("name").text
                _type = t.find("type").text

                # Most bitmasks have a requires attribute used to pull in
                # required '*FlagBits" enum.
                requires = t.attrib.get("requires")
                bitmask = VkBaseType(name, _type, requires=requires)
                bitmasks.append(bitmask)
                type_info["data"] = bitmask

            if type_info["category"] == "define":
                name = t.attrib.get("name")
                define = VkDefine.from_xml(t)
                defines.append(define)
                type_info["data"] = define

            if type_info["category"] == "enum":
                name = t.attrib.get("name")
                # The type section only contains enum names, not the actual definition.
                # Since we already parsed the enum before, just link it in.
                try:
                    type_info["data"] = self.enums[name]
                except KeyError as e:
                    # Not all enums seem to be defined yet, typically that's for
                    # ones ending in 'FlagBits' where future extensions may add
                    # definitions.
                    type_info["data"] = None

            if type_info["category"] == "funcpointer":
                funcpointer = VkFunctionPointer.from_xml(t)
                funcpointers.append(funcpointer)
                type_info["data"] = funcpointer

            if type_info["category"] == "handle":
                name = t.find("name").text
                _type = t.find("type").text
                # Most objects have a parent e.g. VkQueue has VkDevice.
                parent = t.attrib.get("parent")
                handle = VkHandle(name, _type, parent)
                handles.append(handle)
                type_info["data"] = handle

            if type_info["category"] in ["struct", "union"]:
                # We store unions among structs as some structs depend
                # on unions. The types are very similar in parsing and
                # generation anyway. The official vulkan scripts use
                # a similar kind of hack.
                struct = VkStruct.from_xml(t)
                structs.append(struct)
                type_info["data"] = struct

            # Name is in general within a name tag else it is an optional 
            # attribute on the type tag.
            name_elem = t.find("name")
            if name_elem is not None:
                type_info["name"] = name_elem.text
            else:
                type_info["name"] = t.attrib.get("name", None)

            # Store all type data in a shared dictionary, so we can easily
            # look up information for a given type. There are no duplicate
            # names.
            self.types[type_info["name"]] = type_info

        # We need detailed type information during code generation
        # on structs for alignment reasons. Unfortunately structs
        # are parsed among other types, so there is no guarantee
        # that any types needed have been parsed already, so set
        # the data now.
        for struct in structs:
            struct.set_type_info(self.types)

        # Guarantee everything is sorted, so code generation doesn't have
        # to deal with this.
        self.base_types = sorted(base_types, key=lambda base_type: base_type.name)
        self.bitmasks = sorted(bitmasks, key=lambda bitmask: bitmask.name)
        self.defines = defines
        self.funcpointers = sorted(funcpointers, key=lambda fp: fp.name)
        self.handles = sorted(handles, key=lambda handle: handle.name)
        self.structs = sorted(structs, key=lambda struct: struct.name)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-v", "--verbose", action="count", default=0, help="increase output verbosity")

    args = parser.parse_args()
    if args.verbose == 0:
        LOGGER.setLevel(logging.WARNING)
    elif args.verbose == 1:
        LOGGER.setLevel(logging.INFO)
    else: # > 1
        LOGGER.setLevel(logging.DEBUG)

    registry = VkRegistry("vk.xml")
    generator = VkGenerator(registry)

    with open(WINE_VULKAN_H, "w") as f:
        generator.generate_vulkan_h(f)

    with open(WINE_VULKAN_DRIVER_H, "w") as f:
        generator.generate_vulkan_driver_h(f)

    with open(WINE_VULKAN_THUNKS_H, "w") as f:
        generator.generate_thunks_h(f, "wine_")

    with open(WINE_VULKAN_THUNKS_C, "w") as f:
        generator.generate_thunks_c(f, "wine_")

if __name__ == "__main__":
    main()
