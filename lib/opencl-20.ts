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

import {
    cl_bitfield,
    intptr_t,
    Ptr_t,
    cl_uint,
    cl_context,
    cl_device_id,
    cl_command_queue,
    cl_mem,
    cl_sampler,
    cl_kernel,
    cl_event,
    cl_mem_object_type,
    cl_image_format,
    Bitfield
} from "./common";

import {
    QUEUE_PROPERTIES,
    MEM_READ_WRITE,
    MEM_WRITE_ONLY,
    MEM_READ_ONLY,
    MEM_USE_HOST_PTR,
    MEM_ALLOC_HOST_PTR,
    MEM_COPY_HOST_PTR,
    QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
    QUEUE_PROFILING_ENABLE,
    SUCCESS,
    SAMPLER_NORMALIZED_COORDS,
    TRUE,
    FALSE,
    SAMPLER_ADDRESSING_MODE,
    ADDRESS_REPEAT,
    ADDRESS_CLAMP_TO_EDGE,
    ADDRESS_CLAMP,
    ADDRESS_NONE,
    SAMPLER_FILTER_MODE,
    FILTER_NEAREST,
    FILTER_LINEAR,
    MAP_READ,
    MAP_WRITE
} from "./opencl-10";

import {
    ADDRESS_MIRRORED_REPEAT
} from "./opencl-11";

import {
    OpenCL_12_API,
    MEM_HOST_WRITE_ONLY,
    MEM_HOST_READ_ONLY,
    MEM_HOST_NO_ACCESS,
    MAP_WRITE_INVALIDATE_REGION
} from "./opencl-12";

// tslint:disable-next-line: no-empty-interface class-name
export interface OpenCL_20_NOT_IMPLEMENTED {
    // setKernelExecInfo(
    //     kernel: cl_kernel,
    //     param_name: cl_kernel_exec_info,
    //     param_value_size: number,
    //     param_value: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)
    // ): cl_int;
}

// tslint:disable-next-line: class-name
export interface cl_image_desc_v20 {
    image_type: cl_mem_object_type;
    image_width: number;
    image_height: number;
    image_depth: number;
    image_array_size: number;
    image_row_pitch: number;
    image_slice_pitch: number;
    num_mip_levels: cl_uint;
    num_samples: cl_uint;
    buffer: cl_mem;
    mem_object: cl_mem;
}

export type cl_device_svm_capabilities = cl_bitfield & { readonly cl_device_svm_capabilities: unique symbol };
export type cl_queue_properties = cl_bitfield & { readonly cl_queue_properties: unique symbol };
export type cl_svm_mem_flags = cl_bitfield & { readonly cl_svm_mem_flags: unique symbol };
export type cl_pipe_properties = Ptr_t<intptr_t> & { readonly cl_pipe_properties: unique symbol };
export type cl_pipe_info = cl_uint & { readonly cl_pipe_info: unique symbol };
export type cl_sampler_properties = cl_bitfield & { readonly cl_sampler_properties: unique symbol };
export type cl_kernel_exec_info = cl_uint & { readonly cl_kernel_exec_info: unique symbol };

export type INVALID_PIPE_SIZE = -69 & { readonly INVALID_PIPE_SIZE: unique symbol };
export type INVALID_DEVICE_QUEUE = -70 & { readonly INVALID_DEVICE_QUEUE: unique symbol };
export type DEVICE_QUEUE_ON_HOST_PROPERTIES = 0x102A & { readonly DEVICE_QUEUE_ON_HOST_PROPERTIES: unique symbol };
export type DEVICE_IMAGE_PITCH_ALIGNMENT = 0x104A & { readonly DEVICE_IMAGE_PITCH_ALIGNMENT: unique symbol };
export type DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT = 0x104B & { readonly DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT: unique symbol };
export type DEVICE_MAX_READ_WRITE_IMAGE_ARGS = 0x104C & { readonly DEVICE_MAX_READ_WRITE_IMAGE_ARGS: unique symbol };
export type DEVICE_MAX_GLOBAL_VARIABLE_SIZE = 0x104D & { readonly DEVICE_MAX_GLOBAL_VARIABLE_SIZE: unique symbol };
export type DEVICE_QUEUE_ON_DEVICE_PROPERTIES = 0x104E & { readonly DEVICE_QUEUE_ON_DEVICE_PROPERTIES: unique symbol };
export type DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE = 0x104F & { readonly DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE: unique symbol };
export type DEVICE_QUEUE_ON_DEVICE_MAX_SIZE = 0x1050 & { readonly DEVICE_QUEUE_ON_DEVICE_MAX_SIZE: unique symbol };
export type DEVICE_MAX_ON_DEVICE_QUEUES = 0x1051 & { readonly DEVICE_MAX_ON_DEVICE_QUEUES: unique symbol };
export type DEVICE_MAX_ON_DEVICE_EVENTS = 0x1052 & { readonly DEVICE_MAX_ON_DEVICE_EVENTS: unique symbol };
export type DEVICE_SVM_CAPABILITIES = 0x1053 & { readonly DEVICE_SVM_CAPABILITIES: unique symbol };
export type DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE = 0x1054 & { readonly DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE: unique symbol };
export type DEVICE_MAX_PIPE_ARGS = 0x1055 & { readonly DEVICE_MAX_PIPE_ARGS: unique symbol };
export type DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS = 0x1056 & { readonly DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS: unique symbol };
export type DEVICE_PIPE_MAX_PACKET_SIZE = 0x1057 & { readonly DEVICE_PIPE_MAX_PACKET_SIZE: unique symbol };
export type DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT = 0x1058 & { readonly DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT: unique symbol };
export type DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT = 0x1059 & { readonly DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT: unique symbol };
export type DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT = 0x105A & { readonly DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT: unique symbol };
export type QUEUE_ON_DEVICE = 4 & { readonly QUEUE_ON_DEVICE: unique symbol };
export type QUEUE_ON_DEVICE_DEFAULT = 8 & { readonly QUEUE_ON_DEVICE_DEFAULT: unique symbol };

/* cl_device_svm_capabilities */
export type DEVICE_SVM_COARSE_GRAIN_BUFFER = 1 & { readonly DEVICE_SVM_COARSE_GRAIN_BUFFER: unique symbol };
export type DEVICE_SVM_FINE_GRAIN_BUFFER = 2 & { readonly DEVICE_SVM_FINE_GRAIN_BUFFER: unique symbol };
export type DEVICE_SVM_FINE_GRAIN_SYSTEM = 4 & { readonly DEVICE_SVM_FINE_GRAIN_SYSTEM: unique symbol };
export type DEVICE_SVM_ATOMICS = 8 & { readonly DEVICE_SVM_ATOMICS: unique symbol };

export type QUEUE_SIZE = 0x1094 & { readonly QUEUE_SIZE: unique symbol };
export type MEM_SVM_FINE_GRAIN_BUFFER = 1024 & { readonly MEM_SVM_FINE_GRAIN_BUFFER: unique symbol };
export type MEM_SVM_ATOMICS = 2048 & { readonly MEM_SVM_ATOMICS: unique symbol };
export type MEM_KERNEL_READ_AND_WRITE = 4096 & { readonly MEM_KERNEL_READ_AND_WRITE: unique symbol };
export type sRGB = 0x10BF & { readonly sRGB: unique symbol };
export type sRGBx = 0x10C0 & { readonly sRGBx: unique symbol };
export type sRGBA = 0x10C1 & { readonly sRGBA: unique symbol };
export type sBGRA = 0x10C2 & { readonly sBGRA: unique symbol };
export type ABGR = 0x10C3 & { readonly ABGR: unique symbol };
export type MEM_OBJECT_PIPE = 0x10F7 & { readonly MEM_OBJECT_PIPE: unique symbol };
export type MEM_USES_SVM_POINTER = 0x1109 & { readonly MEM_USES_SVM_POINTER: unique symbol };

/* cl_pipe_info */
export type PIPE_PACKET_SIZE = 0x1120 & { readonly PIPE_PACKET_SIZE: unique symbol };
export type PIPE_MAX_PACKETS = 0x1121 & { readonly PIPE_MAX_PACKETS: unique symbol };

/* These enumerants are for the extension: cl_khr_mipmap_image.
   They have since been added to cl_ext.h with an appropriate
   KHR suffix, but are left here for backwards compatibility. */
export type SAMPLER_MIP_FILTER_MODE = 0x1155 & { readonly SAMPLER_MIP_FILTER_MODE: unique symbol };
export type SAMPLER_LOD_MIN = 0x1156 & { readonly SAMPLER_LOD_MIN: unique symbol };
export type SAMPLER_LOD_MAX = 0x1157 & { readonly SAMPLER_LOD_MAX: unique symbol };
export type PROGRAM_BUILD_GLOBAL_VARIABLE_TOTAL_SIZE = 0x1185 & { readonly PROGRAM_BUILD_GLOBAL_VARIABLE_TOTAL_SIZE: unique symbol };

export type KERNEL_ARG_TYPE_PIPE = 8 & { readonly KERNEL_ARG_TYPE_PIPE: unique symbol };

/* cl_kernel_exec_info */
export type KERNEL_EXEC_INFO_SVM_PTRS = 0x11B6 & { readonly KERNEL_EXEC_INFO_SVM_PTRS: unique symbol };
export type KERNEL_EXEC_INFO_SVM_FINE_GRAIN_SYSTEM = 0x11B7 & { readonly KERNEL_EXEC_INFO_SVM_FINE_GRAIN_SYSTEM: unique symbol };

export type COMMAND_SVM_FREE = 0x1209 & { readonly COMMAND_SVM_FREE: unique symbol };
export type COMMAND_SVM_MEMCPY = 0x120A & { readonly COMMAND_SVM_MEMCPY: unique symbol };
export type COMMAND_SVM_MEMFILL = 0x120B & { readonly COMMAND_SVM_MEMFILL: unique symbol };
export type COMMAND_SVM_MAP = 0x120C & { readonly COMMAND_SVM_MAP: unique symbol };
export type COMMAND_SVM_UNMAP = 0x120D & { readonly COMMAND_SVM_UNMAP: unique symbol };
export type PROFILING_COMMAND_COMPLETE = 0x1284 & { readonly PROFILING_COMMAND_COMPLETE: unique symbol };

export type OpenCL_20 = OpenCL_20_API & OpenCL_20_MEMBERS_THAT_ARE_NOT_INCLUDDED_IN_FUTURE_VERSIONS;
export type OpenCL_20_API = OpenCL_20_MEMBERS & OpenCL_20_IMPROVED_MEMBERS;

// tslint:disable-next-line: class-name
export interface OpenCL_20_MEMBERS_THAT_ARE_NOT_INCLUDDED_IN_FUTURE_VERSIONS {
    version: '2.0',
    v10: true,
    v11: true,
    v12: true,
    v20: true,
    v21: false,
    v22: false
}

// tslint:disable-next-line: class-name
export interface OpenCL_20_IMPROVED_MEMBERS {
    getDeviceInfo(
        device: cl_device_id,
        param_name: DEVICE_MAX_PIPE_ARGS
            | DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS
            | DEVICE_PIPE_MAX_PACKET_SIZE
    ): number;

    // This is final version, signature not changed in future OpenCL versions
    createImage(
        context: cl_context,
        flags: Bitfield<MEM_READ_WRITE | MEM_WRITE_ONLY | MEM_READ_ONLY | MEM_USE_HOST_PTR | MEM_ALLOC_HOST_PTR | MEM_COPY_HOST_PTR>,
        image_format: cl_image_format,
        image_desc: cl_image_desc_v20,
        host_ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)
    ): cl_mem;

    getCommandQueueInfo(
        command_queue: cl_command_queue,
        param_name: QUEUE_PROPERTIES
    ): QUEUE_ON_DEVICE_DEFAULT | QUEUE_ON_DEVICE;
}

// tslint:disable-next-line: class-name
export interface OpenCL_20_MEMBERS extends OpenCL_12_API {
    INVALID_PIPE_SIZE: INVALID_PIPE_SIZE;
    INVALID_DEVICE_QUEUE: INVALID_DEVICE_QUEUE;
    DEVICE_QUEUE_ON_HOST_PROPERTIES: DEVICE_QUEUE_ON_HOST_PROPERTIES;
    DEVICE_IMAGE_PITCH_ALIGNMENT: DEVICE_IMAGE_PITCH_ALIGNMENT;
    DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT: DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT;
    DEVICE_MAX_READ_WRITE_IMAGE_ARGS: DEVICE_MAX_READ_WRITE_IMAGE_ARGS;
    DEVICE_MAX_GLOBAL_VARIABLE_SIZE: DEVICE_MAX_GLOBAL_VARIABLE_SIZE;
    DEVICE_QUEUE_ON_DEVICE_PROPERTIES: DEVICE_QUEUE_ON_DEVICE_PROPERTIES;
    DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE: DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE;
    DEVICE_QUEUE_ON_DEVICE_MAX_SIZE: DEVICE_QUEUE_ON_DEVICE_MAX_SIZE;
    DEVICE_MAX_ON_DEVICE_QUEUES: DEVICE_MAX_ON_DEVICE_QUEUES;
    DEVICE_MAX_ON_DEVICE_EVENTS: DEVICE_MAX_ON_DEVICE_EVENTS;
    DEVICE_SVM_CAPABILITIES: DEVICE_SVM_CAPABILITIES;
    DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE: DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE;
    DEVICE_MAX_PIPE_ARGS: DEVICE_MAX_PIPE_ARGS;
    DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS: DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS;
    DEVICE_PIPE_MAX_PACKET_SIZE: DEVICE_PIPE_MAX_PACKET_SIZE;
    DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT: DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT;
    DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT: DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT;
    DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT: DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT;
    QUEUE_ON_DEVICE: QUEUE_ON_DEVICE;
    QUEUE_ON_DEVICE_DEFAULT: QUEUE_ON_DEVICE_DEFAULT;
    DEVICE_SVM_COARSE_GRAIN_BUFFER: DEVICE_SVM_COARSE_GRAIN_BUFFER;
    DEVICE_SVM_FINE_GRAIN_BUFFER: DEVICE_SVM_FINE_GRAIN_BUFFER;
    DEVICE_SVM_FINE_GRAIN_SYSTEM: DEVICE_SVM_FINE_GRAIN_SYSTEM;
    DEVICE_SVM_ATOMICS: DEVICE_SVM_ATOMICS;
    QUEUE_SIZE: QUEUE_SIZE;
    MEM_SVM_FINE_GRAIN_BUFFER: MEM_SVM_FINE_GRAIN_BUFFER;
    MEM_SVM_ATOMICS: MEM_SVM_ATOMICS;
    MEM_KERNEL_READ_AND_WRITE: MEM_KERNEL_READ_AND_WRITE;
    sRGB: sRGB;
    sRGBx: sRGBx;
    sRGBA: sRGBA;
    sBGRA: sBGRA;
    ABGR: ABGR;
    MEM_OBJECT_PIPE: MEM_OBJECT_PIPE;
    MEM_USES_SVM_POINTER: MEM_USES_SVM_POINTER;
    PIPE_PACKET_SIZE: PIPE_PACKET_SIZE;
    PIPE_MAX_PACKETS: PIPE_MAX_PACKETS;
    SAMPLER_MIP_FILTER_MODE: SAMPLER_MIP_FILTER_MODE;
    SAMPLER_LOD_MIN: SAMPLER_LOD_MIN;
    SAMPLER_LOD_MAX: SAMPLER_LOD_MAX;
    PROGRAM_BUILD_GLOBAL_VARIABLE_TOTAL_SIZE: PROGRAM_BUILD_GLOBAL_VARIABLE_TOTAL_SIZE;
    KERNEL_ARG_TYPE_PIPE: KERNEL_ARG_TYPE_PIPE;
    KERNEL_EXEC_INFO_SVM_PTRS: KERNEL_EXEC_INFO_SVM_PTRS;
    KERNEL_EXEC_INFO_SVM_FINE_GRAIN_SYSTEM: KERNEL_EXEC_INFO_SVM_FINE_GRAIN_SYSTEM;
    COMMAND_SVM_FREE: COMMAND_SVM_FREE;
    COMMAND_SVM_MEMCPY: COMMAND_SVM_MEMCPY;
    COMMAND_SVM_MEMFILL: COMMAND_SVM_MEMFILL;
    COMMAND_SVM_MAP: COMMAND_SVM_MAP;
    COMMAND_SVM_UNMAP: COMMAND_SVM_UNMAP;
    PROFILING_COMMAND_COMPLETE: PROFILING_COMMAND_COMPLETE;

    createCommandQueueWithProperties(
        context: cl_context,
        device: cl_device_id,
        properties?: null | undefined | ([]) | (
            [QUEUE_PROPERTIES, Bitfield<QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | QUEUE_PROFILING_ENABLE | QUEUE_ON_DEVICE | QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | QUEUE_ON_DEVICE_DEFAULT | QUEUE_ON_DEVICE>] |
            [QUEUE_SIZE, number]
        ) | (
                [QUEUE_PROPERTIES, Bitfield<QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | QUEUE_PROFILING_ENABLE | QUEUE_ON_DEVICE | QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | QUEUE_ON_DEVICE_DEFAULT | QUEUE_ON_DEVICE>, QUEUE_SIZE, number]
            ) | (
                [QUEUE_SIZE, number, QUEUE_PROPERTIES, Bitfield<QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | QUEUE_PROFILING_ENABLE | QUEUE_ON_DEVICE | QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | QUEUE_ON_DEVICE_DEFAULT | QUEUE_ON_DEVICE>]
            ) | null | undefined
    ): cl_command_queue;

    createPipe(
        context: cl_context,
        flags: Bitfield<MEM_READ_WRITE | MEM_WRITE_ONLY | MEM_READ_ONLY | MEM_USE_HOST_PTR | MEM_ALLOC_HOST_PTR | MEM_COPY_HOST_PTR | MEM_HOST_WRITE_ONLY | MEM_HOST_READ_ONLY | MEM_HOST_NO_ACCESS>,
        pipe_packet_size: cl_uint,
        pipe_max_packets: cl_uint,
        properties: null // As per spec: "In OpenCL 2.0, properties must be NULL"
    ): cl_mem;

    getPipeInfo(
        pipe: cl_mem,
        param_name: PIPE_PACKET_SIZE | PIPE_MAX_PACKETS
    ): number;

    SVMAlloc(
        context: cl_context,
        flags: Bitfield<MEM_READ_WRITE | MEM_WRITE_ONLY | MEM_READ_ONLY | MEM_SVM_FINE_GRAIN_BUFFER | MEM_SVM_ATOMICS>,
        size: number,
        alignment: cl_uint
    ): (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer);

    SVMFree(
        context: cl_context,
        svm_pointer: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)
    ): SUCCESS;

    createSamplerWithProperties(
        context: cl_context,
        sampler_properties: Array<
            [SAMPLER_NORMALIZED_COORDS, TRUE | FALSE | boolean] |
            [SAMPLER_ADDRESSING_MODE, ADDRESS_MIRRORED_REPEAT | ADDRESS_REPEAT | ADDRESS_CLAMP_TO_EDGE | ADDRESS_CLAMP | ADDRESS_NONE] |
            [SAMPLER_FILTER_MODE, FILTER_NEAREST | FILTER_LINEAR]
        >
    ): cl_sampler;

    setKernelArgSVMPointer(
        kernel: cl_kernel,
        arg_index: cl_uint,
        arg_value: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)
    ): SUCCESS;

    enqueueSVMFree<TUserData>(
        command_queue: cl_command_queue,
        svm_pointers: Array<Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer>,
        pfn_free_func: (queue: cl_command_queue, user_data: TUserData) => void,
        user_data: TUserData,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueSVMMemcpy(
        command_queue: cl_command_queue,
        blocking_copy: TRUE | FALSE | boolean,
        dst_ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        src_ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        size: number,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueSVMMemFill(
        command_queue: cl_command_queue,
        svm_ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        pattern: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        size: number,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueSVMMap(
        command_queue: cl_command_queue,
        blocking_map: TRUE | FALSE | boolean,
        flags: Bitfield<MAP_READ | MAP_WRITE | MAP_WRITE_INVALIDATE_REGION>,
        svm_ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        size: number,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueSVMUnmap(
        command_queue: cl_command_queue,
        svm_ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;
}
