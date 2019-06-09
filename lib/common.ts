/*******************************************************************************
 * Copyright (c) 2008-2019 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * KHRONOS STANDARDS. THE UNMODIFIED, NORMATIVE VERSIONS OF KHRONOS
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT
 *    https://www.khronos.org/registry/
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 ******************************************************************************/

export type Ptr<T> = T & { readonly ptr: unique symbol };
export type Ptr_t<T> = T & { readonly ptr_t: unique symbol };
export type Bitfield<T> = number | (T & { readonly Bitfield: unique symbol });

export type cl_int = number & { readonly cl_int: unique symbol };
export type intptr_t = Ptr<number> & { readonly intptr_t: unique symbol };

export type cl_platform_id = Ptr<Object> & { readonly cl_platform_id: unique symbol };
export type cl_device_id = Ptr<Object> & { readonly cl_device_id: unique symbol };
export type cl_context = Ptr<Object> & { readonly cl_context: unique symbol };
export type cl_command_queue = Ptr<Object> & { readonly cl_command_queue: unique symbol };
export type cl_mem = Ptr<Object> & { readonly cl_mem: unique symbol };
export type cl_program = Ptr<Object> & { readonly cl_program: unique symbol };
export type cl_kernel = Ptr<Object> & { readonly cl_kernel: unique symbol };
export type cl_event = Ptr<Object> & { readonly cl_event: unique symbol };
export type cl_sampler = Ptr<Object> & { readonly cl_sampler: unique symbol };

export type cl_uint = number & { readonly cl_uint: unique symbol };
export type cl_ulong = number & { readonly cl_ulong: unique symbol };
export type cl_bitfield = number & { readonly cl_bitfield: unique symbol };

export type cl_bool = cl_uint & { readonly cl_bool: unique symbol };                     /* WARNING!  Unlike cl_ types in cl_platform.h, is: cl_bool not guaranteed to be the same size as the bool in kernels. */
export type cl_device_type = cl_bitfield & { readonly cl_device_type: unique symbol };
export type cl_platform_info = cl_uint & { readonly cl_platform_info: unique symbol };
export type cl_device_info = cl_uint & { readonly cl_device_info: unique symbol };
export type cl_device_fp_config = cl_bitfield & { readonly cl_device_fp_config: unique symbol };
export type cl_device_mem_cache_type = cl_uint & { readonly cl_device_mem_cache_type: unique symbol };
export type cl_device_local_mem_type = cl_uint & { readonly cl_device_local_mem_type: unique symbol };
export type cl_device_exec_capabilities = cl_bitfield & { readonly cl_device_exec_capabilities: unique symbol };

export type cl_command_queue_properties = cl_bitfield;

export type cl_context_properties = Ptr_t<intptr_t>;
export type cl_context_info = cl_uint;

export type cl_command_queue_info = cl_uint;
export type cl_channel_order = cl_uint;
export type cl_channel_type = cl_uint;
export type cl_mem_flags = cl_bitfield;

export type cl_mem_object_type = cl_uint;
export type cl_mem_info = cl_uint;

export type cl_image_info = cl_uint;
export type cl_addressing_mode = cl_uint;
export type cl_filter_mode = cl_uint;
export type cl_sampler_info = cl_uint;
export type cl_map_flags = cl_bitfield;

export type cl_program_info = cl_uint;
export type cl_program_build_info = cl_uint;

export type cl_build_status = cl_int;
export type cl_kernel_info = cl_uint;

export type cl_kernel_work_group_info = cl_uint;

export type cl_event_info = cl_uint;
export type cl_command_type = cl_uint;
export type cl_profiling_info = cl_uint;

export interface cl_image_format {
    image_channel_order: cl_channel_order;
    image_channel_data_type: cl_channel_type;
} // _cl_image_format;
