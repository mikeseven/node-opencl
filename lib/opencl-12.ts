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
    TRUE,
    FALSE,
    FP_DENORM,
    FP_INF_NAN,
    FP_ROUND_TO_NEAREST,
    FP_ROUND_TO_ZERO,
    FP_ROUND_TO_INF,
    FP_FMA,
    DEVICE_TYPE,
    SUCCESS,
    MEM_READ_WRITE,
    MEM_WRITE_ONLY,
    MEM_READ_ONLY,
    MEM_USE_HOST_PTR,
    MEM_ALLOC_HOST_PTR,
    MEM_COPY_HOST_PTR
} from "./opencl-10";

import {
    cl_device_id,
    cl_context,
    cl_image_format,
    cl_mem,
    cl_program,
    cl_platform_id,
    cl_kernel,
    cl_command_queue,
    cl_event,
    cl_mem_object_type,
    Bitfield
} from "./common";
import { OpenCL_11_API } from "./opencl-11";

// export type cl_device_partition_property = Ptr_t<intptr_t> & { readonly cl_device_partition_property: unique symbol };
// export type cl_device_affinity_domain = cl_bitfield & { readonly cl_device_affinity_domain: unique symbol };
// export type cl_program_binary_type = cl_uint & { readonly cl_program_binary_type: unique symbol };
// export type cl_kernel_arg_info = cl_uint & { readonly cl_kernel_arg_info: unique symbol };
// export type cl_kernel_arg_address_qualifier = cl_uint & { readonly cl_kernel_arg_address_qualifier: unique symbol };
// export type cl_kernel_arg_access_qualifier = cl_uint & { readonly cl_kernel_arg_access_qualifier: unique symbol };
// export type cl_kernel_arg_type_qualifier = cl_bitfield & { readonly cl_kernel_arg_type_qualifier: unique symbol };

export type COMPILE_PROGRAM_FAILURE = -15 & { readonly COMPILE_PROGRAM_FAILURE: unique symbol };
export type LINKER_NOT_AVAILABLE = -16 & { readonly LINKER_NOT_AVAILABLE: unique symbol };
export type LINK_PROGRAM_FAILURE = -17 & { readonly LINK_PROGRAM_FAILURE: unique symbol };
export type DEVICE_PARTITION_FAILED = -18 & { readonly DEVICE_PARTITION_FAILED: unique symbol };
export type KERNEL_ARG_INFO_NOT_AVAILABLE = -19 & { readonly KERNEL_ARG_INFO_NOT_AVAILABLE: unique symbol };
export type INVALID_IMAGE_DESCRIPTOR = -65 & { readonly INVALID_IMAGE_DESCRIPTOR: unique symbol };
export type INVALID_COMPILER_OPTIONS = -66 & { readonly INVALID_COMPILER_OPTIONS: unique symbol };
export type INVALID_LINKER_OPTIONS = -67 & { readonly INVALID_LINKER_OPTIONS: unique symbol };
export type INVALID_DEVICE_PARTITION_COUNT = -68 & { readonly INVALID_DEVICE_PARTITION_COUNT: unique symbol };
export type BLOCKING = TRUE & { readonly BLOCKING: unique symbol };
export type NON_BLOCKING = FALSE & { readonly NON_BLOCKING: unique symbol };
export type DEVICE_TYPE_CUSTOM = 16 & { readonly DEVICE_TYPE_CUSTOM: unique symbol };
export type DEVICE_DOUBLE_FP_CONFIG = 0x1032 & { readonly DEVICE_DOUBLE_FP_CONFIG: unique symbol };
export type DEVICE_LINKER_AVAILABLE = 0x103E & { readonly DEVICE_LINKER_AVAILABLE: unique symbol };
export type DEVICE_BUILT_IN_KERNELS = 0x103F & { readonly DEVICE_BUILT_IN_KERNELS: unique symbol };
export type DEVICE_IMAGE_MAX_BUFFER_SIZE = 0x1040 & { readonly DEVICE_IMAGE_MAX_BUFFER_SIZE: unique symbol };
export type DEVICE_IMAGE_MAX_ARRAY_SIZE = 0x1041 & { readonly DEVICE_IMAGE_MAX_ARRAY_SIZE: unique symbol };
export type DEVICE_PARENT_DEVICE = 0x1042 & { readonly DEVICE_PARENT_DEVICE: unique symbol };
export type DEVICE_PARTITION_MAX_SUB_DEVICES = 0x1043 & { readonly DEVICE_PARTITION_MAX_SUB_DEVICES: unique symbol };
export type DEVICE_PARTITION_PROPERTIES = 0x1044 & { readonly DEVICE_PARTITION_PROPERTIES: unique symbol };
export type DEVICE_PARTITION_AFFINITY_DOMAIN = 0x1045 & { readonly DEVICE_PARTITION_AFFINITY_DOMAIN: unique symbol };
export type DEVICE_PARTITION_TYPE = 0x1046 & { readonly DEVICE_PARTITION_TYPE: unique symbol };
export type DEVICE_REFERENCE_COUNT = 0x1047 & { readonly DEVICE_REFERENCE_COUNT: unique symbol };
export type DEVICE_PREFERRED_INTEROP_USER_SYNC = 0x1048 & { readonly DEVICE_PREFERRED_INTEROP_USER_SYNC: unique symbol };
export type DEVICE_PRINTF_BUFFER_SIZE = 0x1049 & { readonly DEVICE_PRINTF_BUFFER_SIZE: unique symbol };
export type FP_CORRECTLY_ROUNDED_DIVIDE_SQRT = 128 & { readonly FP_CORRECTLY_ROUNDED_DIVIDE_SQRT: unique symbol };
export type CONTEXT_INTEROP_USER_SYNC = 0x1085 & { readonly CONTEXT_INTEROP_USER_SYNC: unique symbol };

/* cl_device_partition_property */
export type DEVICE_PARTITION_EQUALLY = 0x1086 & { readonly DEVICE_PARTITION_EQUALLY: unique symbol };
export type DEVICE_PARTITION_BY_COUNTS = 0x1087 & { readonly DEVICE_PARTITION_BY_COUNTS: unique symbol };
export type DEVICE_PARTITION_BY_COUNTS_LIST_END = 0x0 & { readonly DEVICE_PARTITION_BY_COUNTS_LIST_END: unique symbol };
export type DEVICE_PARTITION_BY_AFFINITY_DOMAIN = 0x1088 & { readonly DEVICE_PARTITION_BY_AFFINITY_DOMAIN: unique symbol };

/* cl_device_affinity_domain */
export type DEVICE_AFFINITY_DOMAIN_NUMA = 1 & { readonly DEVICE_AFFINITY_DOMAIN_NUMA: unique symbol };
export type DEVICE_AFFINITY_DOMAIN_L4_CACHE = 2 & { readonly DEVICE_AFFINITY_DOMAIN_L4_CACHE: unique symbol };
export type DEVICE_AFFINITY_DOMAIN_L3_CACHE = 4 & { readonly DEVICE_AFFINITY_DOMAIN_L3_CACHE: unique symbol };
export type DEVICE_AFFINITY_DOMAIN_L2_CACHE = 8 & { readonly DEVICE_AFFINITY_DOMAIN_L2_CACHE: unique symbol };
export type DEVICE_AFFINITY_DOMAIN_L1_CACHE = 16 & { readonly DEVICE_AFFINITY_DOMAIN_L1_CACHE: unique symbol };
export type DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE = 32 & { readonly DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE: unique symbol };

export type MEM_HOST_WRITE_ONLY = 128 & { readonly MEM_HOST_WRITE_ONLY: unique symbol };
export type MEM_HOST_READ_ONLY = 256 & { readonly MEM_HOST_READ_ONLY: unique symbol };
export type MEM_HOST_NO_ACCESS = 512 & { readonly MEM_HOST_NO_ACCESS: unique symbol };

/* cl_mem_migration_flags - bitfield */
export type MIGRATE_MEM_OBJECT_HOST = 1 & { readonly MIGRATE_MEM_OBJECT_HOST: unique symbol };
export type MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED = 2 & { readonly MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED: unique symbol };
export type DEPTH = 0x10BD & { readonly DEPTH: unique symbol };
export type DEPTH_STENCIL = 0x10BE & { readonly DEPTH_STENCIL: unique symbol };
export type UNORM_INT24 = 0x10DF & { readonly UNORM_INT24: unique symbol };
export type MEM_OBJECT_IMAGE2D_ARRAY = 0x10F3 & { readonly MEM_OBJECT_IMAGE2D_ARRAY: unique symbol };
export type MEM_OBJECT_IMAGE1D = 0x10F4 & { readonly MEM_OBJECT_IMAGE1D: unique symbol };
export type MEM_OBJECT_IMAGE1D_ARRAY = 0x10F5 & { readonly MEM_OBJECT_IMAGE1D_ARRAY: unique symbol };
export type MEM_OBJECT_IMAGE1D_BUFFER = 0x10F6 & { readonly MEM_OBJECT_IMAGE1D_BUFFER: unique symbol };
export type IMAGE_ARRAY_SIZE = 0x1117 & { readonly IMAGE_ARRAY_SIZE: unique symbol };
export type IMAGE_BUFFER = 0x1118 & { readonly IMAGE_BUFFER: unique symbol };
export type IMAGE_NUM_MIP_LEVELS = 0x1119 & { readonly IMAGE_NUM_MIP_LEVELS: unique symbol };
export type IMAGE_NUM_SAMPLES = 0x111A & { readonly IMAGE_NUM_SAMPLES: unique symbol };
export type MAP_WRITE_INVALIDATE_REGION = 4 & { readonly MAP_WRITE_INVALIDATE_REGION: unique symbol };
export type PROGRAM_NUM_KERNELS = 0x1167 & { readonly PROGRAM_NUM_KERNELS: unique symbol };
export type PROGRAM_KERNEL_NAMES = 0x1168 & { readonly PROGRAM_KERNEL_NAMES: unique symbol };
export type PROGRAM_BINARY_TYPE = 0x1184 & { readonly PROGRAM_BINARY_TYPE: unique symbol };

/* cl_program_binary_type */
export type PROGRAM_BINARY_TYPE_NONE = 0x0 & { readonly PROGRAM_BINARY_TYPE_NONE: unique symbol };
export type PROGRAM_BINARY_TYPE_COMPILED_OBJECT = 0x1 & { readonly PROGRAM_BINARY_TYPE_COMPILED_OBJECT: unique symbol };
export type PROGRAM_BINARY_TYPE_LIBRARY = 0x2 & { readonly PROGRAM_BINARY_TYPE_LIBRARY: unique symbol };
export type PROGRAM_BINARY_TYPE_EXECUTABLE = 0x4 & { readonly PROGRAM_BINARY_TYPE_EXECUTABLE: unique symbol };

export type KERNEL_ATTRIBUTES = 0x1195 & { readonly KERNEL_ATTRIBUTES: unique symbol };

/* cl_kernel_arg_info */
export type KERNEL_ARG_ADDRESS_QUALIFIER = 0x1196 & { readonly KERNEL_ARG_ADDRESS_QUALIFIER: unique symbol };
export type KERNEL_ARG_ACCESS_QUALIFIER = 0x1197 & { readonly KERNEL_ARG_ACCESS_QUALIFIER: unique symbol };
export type KERNEL_ARG_TYPE_NAME = 0x1198 & { readonly KERNEL_ARG_TYPE_NAME: unique symbol };
export type KERNEL_ARG_TYPE_QUALIFIER = 0x1199 & { readonly KERNEL_ARG_TYPE_QUALIFIER: unique symbol };
export type KERNEL_ARG_NAME = 0x119A & { readonly KERNEL_ARG_NAME: unique symbol };

/* cl_kernel_arg_address_qualifier */
export type KERNEL_ARG_ADDRESS_GLOBAL = 0x119B & { readonly KERNEL_ARG_ADDRESS_GLOBAL: unique symbol };
export type KERNEL_ARG_ADDRESS_LOCAL = 0x119C & { readonly KERNEL_ARG_ADDRESS_LOCAL: unique symbol };
export type KERNEL_ARG_ADDRESS_CONSTANT = 0x119D & { readonly KERNEL_ARG_ADDRESS_CONSTANT: unique symbol };
export type KERNEL_ARG_ADDRESS_PRIVATE = 0x119E & { readonly KERNEL_ARG_ADDRESS_PRIVATE: unique symbol };

/* cl_kernel_arg_access_qualifier */
export type KERNEL_ARG_ACCESS_READ_ONLY = 0x11A0 & { readonly KERNEL_ARG_ACCESS_READ_ONLY: unique symbol };
export type KERNEL_ARG_ACCESS_WRITE_ONLY = 0x11A1 & { readonly KERNEL_ARG_ACCESS_WRITE_ONLY: unique symbol };
export type KERNEL_ARG_ACCESS_READ_WRITE = 0x11A2 & { readonly KERNEL_ARG_ACCESS_READ_WRITE: unique symbol };
export type KERNEL_ARG_ACCESS_NONE = 0x11A3 & { readonly KERNEL_ARG_ACCESS_NONE: unique symbol };

/* cl_kernel_arg_type_qualifier */
export type KERNEL_ARG_TYPE_NONE = 0 & { readonly KERNEL_ARG_TYPE_NONE: unique symbol };
export type KERNEL_ARG_TYPE_CONST = 1 & { readonly KERNEL_ARG_TYPE_CONST: unique symbol };
export type KERNEL_ARG_TYPE_RESTRICT = 2 & { readonly KERNEL_ARG_TYPE_RESTRICT: unique symbol };
export type KERNEL_ARG_TYPE_VOLATILE = 4 & { readonly KERNEL_ARG_TYPE_VOLATILE: unique symbol };

export type KERNEL_GLOBAL_WORK_SIZE = 0x11B5 & { readonly KERNEL_GLOBAL_WORK_SIZE: unique symbol };
export type COMMAND_BARRIER = 0x1205 & { readonly COMMAND_BARRIER: unique symbol };
export type COMMAND_MIGRATE_MEM_OBJECTS = 0x1206 & { readonly COMMAND_MIGRATE_MEM_OBJECTS: unique symbol };
export type COMMAND_FILL_BUFFER = 0x1207 & { readonly COMMAND_FILL_BUFFER: unique symbol };
export type COMMAND_FILL_IMAGE = 0x1208 & { readonly COMMAND_FILL_IMAGE: unique symbol };

// `cl_image_desc` declaration differs in OpenCL_12 and OpenCL_20. So it is moved from
// OpenCL_12_RAW to OpenCL_12 interface to allow for both extension of OpenCL_20_RAW
// interface with OpenCL_12_RAW interface and introduction usage of version-specific
// declaration of `cl_image_desc` in OpenCL_20 interface.

// tslint:disable-next-line: class-name
export interface cl_image_desc_v12 {
    image_type: cl_mem_object_type;
    image_width: number;
    image_height: number;
    image_depth: number;
    image_array_size: number;
    image_row_pitch: number;
    image_slice_pitch: number;
    num_mip_levels: number;
    num_samples: number;
    buffer: cl_mem;
} // _cl_image_desc;

export type OpenCL_12 = OpenCL_12_API & OpenCL_12_MEMBERS_THAT_ARE_NOT_INCLUDDED_IN_FUTURE_VERSIONS;
export type OpenCL_12_API = OpenCL_12_MEMBERS & OpenCL_12_IMPROVED_MEMBERS;

// tslint:disable-next-line: class-name
export interface OpenCL_12_MEMBERS_THAT_ARE_NOT_INCLUDDED_IN_FUTURE_VERSIONS {
    version: '1.2',
    v10: true,
    v11: true,
    v12: true,
    v20: false,
    v21: false,
    v22: false,

    // `createImage` method uses `cl_image_desc` structure as an argument whoose type differs
    // in OpenCL_12 and OpenCL_20. So it is moved from OpenCL_12_RAW to OpenCL_12 interface
    // to allow for cleaner extension of OpenCL_12_RAW interface in OpenCL_20_RAW interface.
    createImage(
        context: cl_context,
        flags: Bitfield<MEM_READ_WRITE | MEM_WRITE_ONLY | MEM_READ_ONLY | MEM_USE_HOST_PTR | MEM_ALLOC_HOST_PTR | MEM_COPY_HOST_PTR>,
        image_format: cl_image_format,
        image_desc: cl_image_desc_v12,
        host_ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)
    ): cl_mem;
}

// tslint:disable-next-line: class-name
export interface OpenCL_12_IMPROVED_MEMBERS {
    getDeviceInfo(device: cl_device_id, param_name: DEVICE_IMAGE_MAX_BUFFER_SIZE | DEVICE_IMAGE_MAX_ARRAY_SIZE | DEVICE_REFERENCE_COUNT | DEVICE_PARTITION_MAX_SUB_DEVICES): number;
    getDeviceInfo(device: cl_device_id, param_name: DEVICE_DOUBLE_FP_CONFIG): FP_DENORM | FP_INF_NAN | FP_ROUND_TO_NEAREST | FP_ROUND_TO_ZERO | FP_ROUND_TO_INF | FP_FMA;
    getDeviceInfo(device: cl_device_id, param_name: DEVICE_TYPE): DEVICE_TYPE_CUSTOM;

    getImageInfo(
        image: cl_mem,
        param_name: IMAGE_ARRAY_SIZE
    ): number;
    getImageInfo(
        image: cl_mem,
        param_name: IMAGE_BUFFER
    ): cl_mem;
    getImageInfo(
        image: cl_mem,
        // tslint:disable-next-line: unified-signatures
        param_name: IMAGE_NUM_MIP_LEVELS | IMAGE_NUM_SAMPLES
    ): number;
    getProgramInfo(
        program: cl_program,
        param_name: PROGRAM_NUM_KERNELS
    ): number;
    getProgramBuildInfo(
        program: cl_program,
        device: cl_device_id,
        param_name: PROGRAM_BINARY_TYPE
    ): PROGRAM_BINARY_TYPE_NONE | PROGRAM_BINARY_TYPE_COMPILED_OBJECT | PROGRAM_BINARY_TYPE_LIBRARY | PROGRAM_BINARY_TYPE_EXECUTABLE;
    setKernelArg(
        kernel: cl_kernel,
        arg_index: number,
        arg_type: string | null | undefined,
        arg_value: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)
    ): SUCCESS;
    getKernelInfo(
        kernel: cl_kernel,
        param_name: KERNEL_ATTRIBUTES
    ): SUCCESS;
    getKernelWorkGroupInfo(
        kernel: cl_kernel,
        device: cl_device_id,
        param_name: KERNEL_GLOBAL_WORK_SIZE
    ): [number, number, number];
}

// tslint:disable-next-line: class-name
export interface OpenCL_12_MEMBERS extends OpenCL_11_API {
    COMPILE_PROGRAM_FAILURE: COMPILE_PROGRAM_FAILURE;
    LINKER_NOT_AVAILABLE: LINKER_NOT_AVAILABLE;
    LINK_PROGRAM_FAILURE: LINK_PROGRAM_FAILURE;
    DEVICE_PARTITION_FAILED: DEVICE_PARTITION_FAILED;
    KERNEL_ARG_INFO_NOT_AVAILABLE: KERNEL_ARG_INFO_NOT_AVAILABLE;
    INVALID_IMAGE_DESCRIPTOR: INVALID_IMAGE_DESCRIPTOR;
    INVALID_COMPILER_OPTIONS: INVALID_COMPILER_OPTIONS;
    INVALID_LINKER_OPTIONS: INVALID_LINKER_OPTIONS;
    INVALID_DEVICE_PARTITION_COUNT: INVALID_DEVICE_PARTITION_COUNT;
    BLOCKING: BLOCKING;
    NON_BLOCKING: NON_BLOCKING;
    DEVICE_TYPE_CUSTOM: DEVICE_TYPE_CUSTOM;
    DEVICE_DOUBLE_FP_CONFIG: DEVICE_DOUBLE_FP_CONFIG;
    DEVICE_LINKER_AVAILABLE: DEVICE_LINKER_AVAILABLE;
    DEVICE_BUILT_IN_KERNELS: DEVICE_BUILT_IN_KERNELS;
    DEVICE_IMAGE_MAX_BUFFER_SIZE: DEVICE_IMAGE_MAX_BUFFER_SIZE;
    DEVICE_IMAGE_MAX_ARRAY_SIZE: DEVICE_IMAGE_MAX_ARRAY_SIZE;
    DEVICE_PARENT_DEVICE: DEVICE_PARENT_DEVICE;
    DEVICE_PARTITION_MAX_SUB_DEVICES: DEVICE_PARTITION_MAX_SUB_DEVICES;
    DEVICE_PARTITION_PROPERTIES: DEVICE_PARTITION_PROPERTIES;
    DEVICE_PARTITION_AFFINITY_DOMAIN: DEVICE_PARTITION_AFFINITY_DOMAIN;
    DEVICE_PARTITION_TYPE: DEVICE_PARTITION_TYPE;
    DEVICE_REFERENCE_COUNT: DEVICE_REFERENCE_COUNT;
    DEVICE_PREFERRED_INTEROP_USER_SYNC: DEVICE_PREFERRED_INTEROP_USER_SYNC;
    DEVICE_PRINTF_BUFFER_SIZE: DEVICE_PRINTF_BUFFER_SIZE;
    FP_CORRECTLY_ROUNDED_DIVIDE_SQRT: FP_CORRECTLY_ROUNDED_DIVIDE_SQRT;
    CONTEXT_INTEROP_USER_SYNC: CONTEXT_INTEROP_USER_SYNC;
    DEVICE_PARTITION_EQUALLY: DEVICE_PARTITION_EQUALLY;
    DEVICE_PARTITION_BY_COUNTS: DEVICE_PARTITION_BY_COUNTS;
    DEVICE_PARTITION_BY_COUNTS_LIST_END: DEVICE_PARTITION_BY_COUNTS_LIST_END;
    DEVICE_PARTITION_BY_AFFINITY_DOMAIN: DEVICE_PARTITION_BY_AFFINITY_DOMAIN;
    DEVICE_AFFINITY_DOMAIN_NUMA: DEVICE_AFFINITY_DOMAIN_NUMA;
    DEVICE_AFFINITY_DOMAIN_L4_CACHE: DEVICE_AFFINITY_DOMAIN_L4_CACHE;
    DEVICE_AFFINITY_DOMAIN_L3_CACHE: DEVICE_AFFINITY_DOMAIN_L3_CACHE;
    DEVICE_AFFINITY_DOMAIN_L2_CACHE: DEVICE_AFFINITY_DOMAIN_L2_CACHE;
    DEVICE_AFFINITY_DOMAIN_L1_CACHE: DEVICE_AFFINITY_DOMAIN_L1_CACHE;
    DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE: DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE;
    MEM_HOST_WRITE_ONLY: MEM_HOST_WRITE_ONLY;
    MEM_HOST_READ_ONLY: MEM_HOST_READ_ONLY;
    MEM_HOST_NO_ACCESS: MEM_HOST_NO_ACCESS;
    MIGRATE_MEM_OBJECT_HOST: MIGRATE_MEM_OBJECT_HOST;
    MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED: MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED;
    DEPTH: DEPTH;
    DEPTH_STENCIL: DEPTH_STENCIL;
    UNORM_INT24: UNORM_INT24;
    MEM_OBJECT_IMAGE2D_ARRAY: MEM_OBJECT_IMAGE2D_ARRAY;
    MEM_OBJECT_IMAGE1D: MEM_OBJECT_IMAGE1D;
    MEM_OBJECT_IMAGE1D_ARRAY: MEM_OBJECT_IMAGE1D_ARRAY;
    MEM_OBJECT_IMAGE1D_BUFFER: MEM_OBJECT_IMAGE1D_BUFFER;
    IMAGE_ARRAY_SIZE: IMAGE_ARRAY_SIZE;
    IMAGE_BUFFER: IMAGE_BUFFER;
    IMAGE_NUM_MIP_LEVELS: IMAGE_NUM_MIP_LEVELS;
    IMAGE_NUM_SAMPLES: IMAGE_NUM_SAMPLES;
    MAP_WRITE_INVALIDATE_REGION: MAP_WRITE_INVALIDATE_REGION;
    PROGRAM_NUM_KERNELS: PROGRAM_NUM_KERNELS;
    PROGRAM_KERNEL_NAMES: PROGRAM_KERNEL_NAMES;
    PROGRAM_BINARY_TYPE: PROGRAM_BINARY_TYPE;
    PROGRAM_BINARY_TYPE_NONE: PROGRAM_BINARY_TYPE_NONE;
    PROGRAM_BINARY_TYPE_COMPILED_OBJECT: PROGRAM_BINARY_TYPE_COMPILED_OBJECT;
    PROGRAM_BINARY_TYPE_LIBRARY: PROGRAM_BINARY_TYPE_LIBRARY;
    PROGRAM_BINARY_TYPE_EXECUTABLE: PROGRAM_BINARY_TYPE_EXECUTABLE;
    KERNEL_ATTRIBUTES: KERNEL_ATTRIBUTES;
    KERNEL_ARG_ADDRESS_QUALIFIER: KERNEL_ARG_ADDRESS_QUALIFIER;
    KERNEL_ARG_ACCESS_QUALIFIER: KERNEL_ARG_ACCESS_QUALIFIER;
    KERNEL_ARG_TYPE_NAME: KERNEL_ARG_TYPE_NAME;
    KERNEL_ARG_TYPE_QUALIFIER: KERNEL_ARG_TYPE_QUALIFIER;
    KERNEL_ARG_NAME: KERNEL_ARG_NAME;
    KERNEL_ARG_ADDRESS_GLOBAL: KERNEL_ARG_ADDRESS_GLOBAL;
    KERNEL_ARG_ADDRESS_LOCAL: KERNEL_ARG_ADDRESS_LOCAL;
    KERNEL_ARG_ADDRESS_CONSTANT: KERNEL_ARG_ADDRESS_CONSTANT;
    KERNEL_ARG_ADDRESS_PRIVATE: KERNEL_ARG_ADDRESS_PRIVATE;
    KERNEL_ARG_ACCESS_READ_ONLY: KERNEL_ARG_ACCESS_READ_ONLY;
    KERNEL_ARG_ACCESS_WRITE_ONLY: KERNEL_ARG_ACCESS_WRITE_ONLY;
    KERNEL_ARG_ACCESS_READ_WRITE: KERNEL_ARG_ACCESS_READ_WRITE;
    KERNEL_ARG_ACCESS_NONE: KERNEL_ARG_ACCESS_NONE;
    KERNEL_ARG_TYPE_NONE: KERNEL_ARG_TYPE_NONE;
    KERNEL_ARG_TYPE_CONST: KERNEL_ARG_TYPE_CONST;
    KERNEL_ARG_TYPE_RESTRICT: KERNEL_ARG_TYPE_RESTRICT;
    KERNEL_ARG_TYPE_VOLATILE: KERNEL_ARG_TYPE_VOLATILE;
    KERNEL_GLOBAL_WORK_SIZE: KERNEL_GLOBAL_WORK_SIZE;
    COMMAND_BARRIER: COMMAND_BARRIER;
    COMMAND_MIGRATE_MEM_OBJECTS: COMMAND_MIGRATE_MEM_OBJECTS;
    COMMAND_FILL_BUFFER: COMMAND_FILL_BUFFER;
    COMMAND_FILL_IMAGE: COMMAND_FILL_IMAGE;

    createSubDevices(
        in_device: cl_device_id,
        properties: Array<DEVICE_PARTITION_EQUALLY |
            DEVICE_PARTITION_BY_COUNTS |
            DEVICE_PARTITION_BY_COUNTS_LIST_END |
            DEVICE_PARTITION_BY_AFFINITY_DOMAIN>
    ): cl_device_id[];

    retainDevice(device: cl_device_id): SUCCESS;
    releaseDevice(device: cl_device_id): SUCCESS;

    createProgramWithBuiltInKernels(
        context: cl_context,
        device_list: cl_device_id[],
        kernel_names: string
    ): cl_program;

    compileProgram<TUserData>(
        program: cl_program,
        device_list?: cl_device_id[] | null | undefined,
        options?: string | null | undefined,
        input_headers?: cl_program[] | null | undefined,
        header_include_names?: string[] | null | undefined,
        pfn_notify?: (program: cl_program, user_data: TUserData) => void | null | undefined,
        user_data?: TUserData | null | undefined
    ): SUCCESS;

    linkProgram<TUserData>(
        context: cl_context,
        device_list: cl_device_id,
        options: string,
        input_programs: cl_program[],
        pfn_notify?: (program: cl_program, user_data: TUserData) => void | null | undefined,
        user_data?: TUserData
    ): cl_program;

    unloadPlatformCompiler(platform: cl_platform_id): SUCCESS;

    getKernelArgInfo(
        kernel: cl_kernel,
        arg_indx: number,
        param_name: KERNEL_ARG_ADDRESS_QUALIFIER
    ): KERNEL_ARG_ADDRESS_GLOBAL | KERNEL_ARG_ADDRESS_LOCAL | KERNEL_ARG_ADDRESS_CONSTANT | KERNEL_ARG_ADDRESS_PRIVATE;
    getKernelArgInfo(
        kernel: cl_kernel,
        arg_indx: number,
        param_name: KERNEL_ARG_ACCESS_QUALIFIER
    ): KERNEL_ARG_ACCESS_READ_ONLY | KERNEL_ARG_ACCESS_WRITE_ONLY | KERNEL_ARG_ACCESS_READ_WRITE | KERNEL_ARG_ACCESS_NONE;
    getKernelArgInfo(
        kernel: cl_kernel,
        arg_indx: number,
        param_name: KERNEL_ARG_TYPE_QUALIFIER
    ): Bitfield<KERNEL_ARG_TYPE_CONST | KERNEL_ARG_TYPE_RESTRICT | KERNEL_ARG_TYPE_VOLATILE>;
    getKernelArgInfo(
        kernel: cl_kernel,
        arg_indx: number,
        param_name: KERNEL_ARG_TYPE_NAME | KERNEL_ARG_NAME
    ): string;

    enqueueFillBuffer(
        command_queue: cl_command_queue,
        buffer: cl_mem,
        pattern: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | number,
        offset: number,
        size: number,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueFillImage(
        command_queue: cl_command_queue,
        image: cl_mem,
        fill_color: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        origin: [number, number, number],
        region: [number, number, number],
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;
    
    enqueueFillImage(
        command_queue: cl_command_queue,
        image: cl_mem,
        fill_color: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        origin: [number, number, number],
        region: [number, number, number],
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueMigrateMemObjects(
        command_queue: cl_command_queue,
        mem_objects: cl_mem[],
        flags: MIGRATE_MEM_OBJECT_HOST | MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueMarkerWithWaitList(
        command_queue: cl_command_queue,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueBarrierWithWaitList(
        command_queue: cl_command_queue,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    /* Extension function access
     *
     * Returns the extension function address for the given function name,
     * or NULL if a valid function can not be found.  The client must
     * check to make sure the address is not NULL, before using or
     * calling the returned function address.
     */
    // Not implemented
    // getExtensionFunctionAddressForPlatform(platform: cl_platform_id, func_name: string): (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer);
}
