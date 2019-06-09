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
    cl_mem,
    cl_event,
    cl_kernel,
    cl_program,
    cl_sampler,
    cl_device_id,
    cl_platform_id,
    cl_context,
    cl_context_properties,
    cl_image_format,
    cl_command_queue,
    Bitfield
} from "./common";

import { OpenCL_NodeExtensions as NodeOpenCL_EXTENSIONS } from "./node-opencl-extensions";

// tslint:disable class-name
// tslint:disable-next-line: no-empty-interface class-name
export interface OpenCL_10_NOT_IMPLEMENTED {
    // cl_int clEnqueueNativeKernel(cl_command_queue command_queue,
    //      void (* user_func)(void *),
    //      void * args,
    //      number cb_args,
    //      cl_uint num_mem_objects,
    //      const cl_mem * mem_list,
    //      const void ** args_mem_loc,
    //      cl_uint num_events_in_wait_list,
    //      const cl_event * event_wait_list,
    //      cl_event * event
    // )
}

/* Error Codes */
export type SUCCESS = 0 & { readonly SUCCESS: unique symbol };
export type DEVICE_NOT_FOUND = -1 & { readonly DEVICE_NOT_FOUND: unique symbol };
export type DEVICE_NOT_AVAILABLE = -2 & { readonly DEVICE_NOT_AVAILABLE: unique symbol };
export type COMPILER_NOT_AVAILABLE = -3 & { readonly COMPILER_NOT_AVAILABLE: unique symbol };
export type MEM_OBJECT_ALLOCATION_FAILURE = -4 & { readonly MEM_OBJECT_ALLOCATION_FAILURE: unique symbol };
export type OUT_OF_RESOURCES = -5 & { readonly OUT_OF_RESOURCES: unique symbol };
export type OUT_OF_HOST_MEMORY = -6 & { readonly OUT_OF_HOST_MEMORY: unique symbol };
export type PROFILING_INFO_NOT_AVAILABLE = -7 & { readonly PROFILING_INFO_NOT_AVAILABLE: unique symbol };
export type MEM_COPY_OVERLAP = -8 & { readonly MEM_COPY_OVERLAP: unique symbol };
export type IMAGE_FORMAT_MISMATCH = -9 & { readonly IMAGE_FORMAT_MISMATCH: unique symbol };
export type IMAGE_FORMAT_NOT_SUPPORTED = -10 & { readonly IMAGE_FORMAT_NOT_SUPPORTED: unique symbol };
export type BUILD_PROGRAM_FAILURE = -11 & { readonly BUILD_PROGRAM_FAILURE: unique symbol };
export type MAP_FAILURE = -12 & { readonly MAP_FAILURE: unique symbol };

export type INVALID_VALUE = -30 & { readonly INVALID_VALUE: unique symbol };
export type INVALID_DEVICE_TYPE = -31 & { readonly INVALID_DEVICE_TYPE: unique symbol };
export type INVALID_PLATFORM = -32 & { readonly INVALID_PLATFORM: unique symbol };
export type INVALID_DEVICE = -33 & { readonly INVALID_DEVICE: unique symbol };
export type INVALID_CONTEXT = -34 & { readonly INVALID_CONTEXT: unique symbol };
export type INVALID_QUEUE_PROPERTIES = -35 & { readonly INVALID_QUEUE_PROPERTIES: unique symbol };
export type INVALID_COMMAND_QUEUE = -36 & { readonly INVALID_COMMAND_QUEUE: unique symbol };
export type INVALID_HOST_PTR = -37 & { readonly INVALID_HOST_PTR: unique symbol };
export type INVALID_MEM_OBJECT = -38 & { readonly INVALID_MEM_OBJECT: unique symbol };
export type INVALID_IMAGE_FORMAT_DESCRIPTOR = -39 & { readonly INVALID_IMAGE_FORMAT_DESCRIPTOR: unique symbol };
export type INVALID_IMAGE_SIZE = -40 & { readonly INVALID_IMAGE_SIZE: unique symbol };
export type INVALID_SAMPLER = -41 & { readonly INVALID_SAMPLER: unique symbol };
export type INVALID_BINARY = -42 & { readonly INVALID_BINARY: unique symbol };
export type INVALID_BUILD_OPTIONS = -43 & { readonly INVALID_BUILD_OPTIONS: unique symbol };
export type INVALID_PROGRAM = -44 & { readonly INVALID_PROGRAM: unique symbol };
export type INVALID_PROGRAM_EXECUTABLE = -45 & { readonly INVALID_PROGRAM_EXECUTABLE: unique symbol };
export type INVALID_KERNEL_NAME = -46 & { readonly INVALID_KERNEL_NAME: unique symbol };
export type INVALID_KERNEL_DEFINITION = -47 & { readonly INVALID_KERNEL_DEFINITION: unique symbol };
export type INVALID_KERNEL = -48 & { readonly INVALID_KERNEL: unique symbol };
export type INVALID_ARG_INDEX = -49 & { readonly INVALID_ARG_INDEX: unique symbol };
export type INVALID_ARG_VALUE = -50 & { readonly INVALID_ARG_VALUE: unique symbol };
export type INVALID_ARG_SIZE = -51 & { readonly INVALID_ARG_SIZE: unique symbol };
export type INVALID_KERNEL_ARGS = -52 & { readonly INVALID_KERNEL_ARGS: unique symbol };
export type INVALID_WORK_DIMENSION = -53 & { readonly INVALID_WORK_DIMENSION: unique symbol };
export type INVALID_WORK_GROUP_SIZE = -54 & { readonly INVALID_WORK_GROUP_SIZE: unique symbol };
export type INVALID_WORK_ITEM_SIZE = -55 & { readonly INVALID_WORK_ITEM_SIZE: unique symbol };
export type INVALID_GLOBAL_OFFSET = -56 & { readonly INVALID_GLOBAL_OFFSET: unique symbol };
export type INVALID_EVENT_WAIT_LIST = -57 & { readonly INVALID_EVENT_WAIT_LIST: unique symbol };
export type INVALID_EVENT = -58 & { readonly INVALID_EVENT: unique symbol };
export type INVALID_OPERATION = -59 & { readonly INVALID_OPERATION: unique symbol };
export type INVALID_GL_OBJECT = -60 & { readonly INVALID_GL_OBJECT: unique symbol };
export type INVALID_BUFFER_SIZE = -61 & { readonly INVALID_BUFFER_SIZE: unique symbol };
export type INVALID_MIP_LEVEL = -62 & { readonly INVALID_MIP_LEVEL: unique symbol };
export type INVALID_GLOBAL_WORK_SIZE = -63 & { readonly INVALID_GLOBAL_WORK_SIZE: unique symbol };

/* cl_bool */
export type FALSE = 0 & { readonly FALSE: unique symbol };
export type TRUE = 1 & { readonly TRUE: unique symbol };

/* cl_platform_info */
export type PLATFORM_PROFILE = 0x0900 & { readonly PLATFORM_PROFILE: unique symbol };
export type PLATFORM_VERSION = 0x0901 & { readonly PLATFORM_VERSION: unique symbol };
export type PLATFORM_NAME = 0x0902 & { readonly PLATFORM_NAME: unique symbol };
export type PLATFORM_VENDOR = 0x0903 & { readonly PLATFORM_VENDOR: unique symbol };
export type PLATFORM_EXTENSIONS = 0x0904 & { readonly PLATFORM_EXTENSIONS: unique symbol };

/* cl_device_type - bitfield */
export type DEVICE_TYPE_DEFAULT = 1 & { readonly DEVICE_TYPE_DEFAULT: unique symbol };
export type DEVICE_TYPE_CPU = 2 & { readonly DEVICE_TYPE_CPU: unique symbol };
export type DEVICE_TYPE_GPU = 4 & { readonly DEVICE_TYPE_GPU: unique symbol };
export type DEVICE_TYPE_ACCELERATOR = 8 & { readonly DEVICE_TYPE_ACCELERATOR: unique symbol };

export type DEVICE_TYPE_ALL = 0xFFFFFFFF & { readonly DEVICE_TYPE_ALL: unique symbol };

/* cl_device_info */
export type DEVICE_TYPE = 0x1000 & { readonly DEVICE_TYPE: unique symbol };
export type DEVICE_VENDOR_ID = 0x1001 & { readonly DEVICE_VENDOR_ID: unique symbol };
export type DEVICE_MAX_COMPUTE_UNITS = 0x1002 & { readonly DEVICE_MAX_COMPUTE_UNITS: unique symbol };
export type DEVICE_MAX_WORK_ITEM_DIMENSIONS = 0x1003 & { readonly DEVICE_MAX_WORK_ITEM_DIMENSIONS: unique symbol };
export type DEVICE_MAX_WORK_GROUP_SIZE = 0x1004 & { readonly DEVICE_MAX_WORK_GROUP_SIZE: unique symbol };
export type DEVICE_MAX_WORK_ITEM_SIZES = 0x1005 & { readonly DEVICE_MAX_WORK_ITEM_SIZES: unique symbol };
export type DEVICE_PREFERRED_VECTOR_WIDTH_CHAR = 0x1006 & { readonly DEVICE_PREFERRED_VECTOR_WIDTH_CHAR: unique symbol };
export type DEVICE_PREFERRED_VECTOR_WIDTH_SHORT = 0x1007 & { readonly DEVICE_PREFERRED_VECTOR_WIDTH_SHORT: unique symbol };
export type DEVICE_PREFERRED_VECTOR_WIDTH_INT = 0x1008 & { readonly DEVICE_PREFERRED_VECTOR_WIDTH_INT: unique symbol };
export type DEVICE_PREFERRED_VECTOR_WIDTH_LONG = 0x1009 & { readonly DEVICE_PREFERRED_VECTOR_WIDTH_LONG: unique symbol };
export type DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT = 0x100A & { readonly DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT: unique symbol };
export type DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE = 0x100B & { readonly DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE: unique symbol };
export type DEVICE_MAX_CLOCK_FREQUENCY = 0x100C & { readonly DEVICE_MAX_CLOCK_FREQUENCY: unique symbol };
export type DEVICE_ADDRESS_BITS = 0x100D & { readonly DEVICE_ADDRESS_BITS: unique symbol };
export type DEVICE_MAX_READ_IMAGE_ARGS = 0x100E & { readonly DEVICE_MAX_READ_IMAGE_ARGS: unique symbol };
export type DEVICE_MAX_WRITE_IMAGE_ARGS = 0x100F & { readonly DEVICE_MAX_WRITE_IMAGE_ARGS: unique symbol };
export type DEVICE_MAX_MEM_ALLOC_SIZE_KILOBYTES = 0x1010 & { readonly DEVICE_MAX_MEM_ALLOC_SIZE_MEGABYTES: unique symbol };
export type DEVICE_IMAGE2D_MAX_WIDTH = 0x1011 & { readonly DEVICE_IMAGE2D_MAX_WIDTH: unique symbol };
export type DEVICE_IMAGE2D_MAX_HEIGHT = 0x1012 & { readonly DEVICE_IMAGE2D_MAX_HEIGHT: unique symbol };
export type DEVICE_IMAGE3D_MAX_WIDTH = 0x1013 & { readonly DEVICE_IMAGE3D_MAX_WIDTH: unique symbol };
export type DEVICE_IMAGE3D_MAX_HEIGHT = 0x1014 & { readonly DEVICE_IMAGE3D_MAX_HEIGHT: unique symbol };
export type DEVICE_IMAGE3D_MAX_DEPTH = 0x1015 & { readonly DEVICE_IMAGE3D_MAX_DEPTH: unique symbol };
export type DEVICE_IMAGE_SUPPORT = 0x1016 & { readonly DEVICE_IMAGE_SUPPORT: unique symbol };
export type DEVICE_MAX_PARAMETER_SIZE = 0x1017 & { readonly DEVICE_MAX_PARAMETER_SIZE: unique symbol };
export type DEVICE_MAX_SAMPLERS = 0x1018 & { readonly DEVICE_MAX_SAMPLERS: unique symbol };
export type DEVICE_MEM_BASE_ADDR_ALIGN = 0x1019 & { readonly DEVICE_MEM_BASE_ADDR_ALIGN: unique symbol };
export type DEVICE_MIN_DATA_TYPE_ALIGN_SIZE = 0x101A & { readonly DEVICE_MIN_DATA_TYPE_ALIGN_SIZE: unique symbol };
export type DEVICE_HALF_FP_CONFIG = 0x101B & { readonly DEVICE_HALF_FP_CONFIG: unique symbol };
export type DEVICE_SINGLE_FP_CONFIG = 0x101B & { readonly DEVICE_SINGLE_FP_CONFIG: unique symbol };
export type DEVICE_GLOBAL_MEM_CACHE_TYPE = 0x101C & { readonly DEVICE_GLOBAL_MEM_CACHE_TYPE: unique symbol };
export type DEVICE_GLOBAL_MEM_CACHELINE_SIZE = 0x101D & { readonly DEVICE_GLOBAL_MEM_CACHELINE_SIZE: unique symbol };
export type DEVICE_GLOBAL_MEM_CACHE_SIZE_KILOBYTES = 0x101E & { readonly DEVICE_GLOBAL_MEM_CACHE_SIZE: unique symbol };
export type DEVICE_GLOBAL_MEM_SIZE_KILOBYTES = 0x101F & { readonly DEVICE_GLOBAL_MEM_SIZE_MEGABYTES: unique symbol };
export type DEVICE_MAX_CONSTANT_BUFFER_SIZE_KILOBYTES = 0x1020 & { readonly DEVICE_MAX_CONSTANT_BUFFER_SIZE: unique symbol };
export type DEVICE_MAX_CONSTANT_ARGS = 0x1021 & { readonly DEVICE_MAX_CONSTANT_ARGS: unique symbol };
export type DEVICE_LOCAL_MEM_TYPE = 0x1022 & { readonly DEVICE_LOCAL_MEM_TYPE: unique symbol };
export type DEVICE_LOCAL_MEM_SIZE_KILOBYTES = 0x1023 & { readonly DEVICE_LOCAL_MEM_SIZE: unique symbol };
export type DEVICE_ERROR_CORRECTION_SUPPORT = 0x1024 & { readonly DEVICE_ERROR_CORRECTION_SUPPORT: unique symbol };
export type DEVICE_PROFILING_TIMER_RESOLUTION = 0x1025 & { readonly DEVICE_PROFILING_TIMER_RESOLUTION: unique symbol };
export type DEVICE_ENDIAN_LITTLE = 0x1026 & { readonly DEVICE_ENDIAN_LITTLE: unique symbol };
export type DEVICE_AVAILABLE = 0x1027 & { readonly DEVICE_AVAILABLE: unique symbol };
export type DEVICE_COMPILER_AVAILABLE = 0x1028 & { readonly DEVICE_COMPILER_AVAILABLE: unique symbol };
export type DEVICE_EXECUTION_CAPABILITIES = 0x1029 & { readonly DEVICE_EXECUTION_CAPABILITIES: unique symbol };

// deprecated in 2.0, not yet removed
export type DEVICE_QUEUE_PROPERTIES = 0x102A & { readonly DEVICE_QUEUE_PROPERTIES: unique symbol };

export type DEVICE_NAME = 0x102B & { readonly DEVICE_NAME: unique symbol };
export type DEVICE_VENDOR = 0x102C & { readonly DEVICE_VENDOR: unique symbol };
export type DRIVER_VERSION = 0x102D & { readonly DRIVER_VERSION: unique symbol };
export type DEVICE_PROFILE = 0x102E & { readonly DEVICE_PROFILE: unique symbol };
export type DEVICE_VERSION = 0x102F & { readonly DEVICE_VERSION: unique symbol };
export type DEVICE_EXTENSIONS = 0x1030 & { readonly DEVICE_EXTENSIONS: unique symbol };
export type DEVICE_PLATFORM = 0x1031 & { readonly DEVICE_PLATFORM: unique symbol };

/* 0x1033 reserved for which: DEVICE_HALF_FP_CONFIG is already defined in "cl_ext.h" */

/* cl_device_fp_config - bitfield */
export type FP_DENORM = 1 & { readonly FP_DENORM: unique symbol };
export type FP_INF_NAN = 2 & { readonly FP_INF_NAN: unique symbol };
export type FP_ROUND_TO_NEAREST = 4 & { readonly FP_ROUND_TO_NEAREST: unique symbol };
export type FP_ROUND_TO_ZERO = 8 & { readonly FP_ROUND_TO_ZERO: unique symbol };
export type FP_ROUND_TO_INF = 16 & { readonly FP_ROUND_TO_INF: unique symbol };
export type FP_FMA = 32 & { readonly FP_FMA: unique symbol };

/* cl_device_mem_cache_type */
export type NONE = 0x0 & { readonly NONE: unique symbol };
export type READ_ONLY_CACHE = 0x1 & { readonly READ_ONLY_CACHE: unique symbol };
export type READ_WRITE_CACHE = 0x2 & { readonly READ_WRITE_CACHE: unique symbol };

/* cl_device_local_mem_type */
export type LOCAL = 0x1 & { readonly LOCAL: unique symbol };
export type GLOBAL = 0x2 & { readonly GLOBAL: unique symbol };

/* cl_device_exec_capabilities - bitfield */
export type EXEC_KERNEL = 1 & { readonly EXEC_KERNEL: unique symbol };
export type EXEC_NATIVE_KERNEL = 2 & { readonly EXEC_NATIVE_KERNEL: unique symbol };

/* cl_command_queue_properties - bitfield */
export type QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE = 1 & { readonly QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE: unique symbol };
export type QUEUE_PROFILING_ENABLE = 2 & { readonly QUEUE_PROFILING_ENABLE: unique symbol };

/* cl_context_info */
export type CONTEXT_REFERENCE_COUNT = 0x1080 & { readonly CONTEXT_REFERENCE_COUNT: unique symbol };
export type CONTEXT_DEVICES = 0x1081 & { readonly CONTEXT_DEVICES: unique symbol };
export type CONTEXT_PROPERTIES = 0x1082 & { readonly CONTEXT_PROPERTIES: unique symbol };

/* cl_context_properties */
export type CONTEXT_PLATFORM = 0x1084 & { readonly CONTEXT_PLATFORM: unique symbol };

/* cl_command_queue_info */
export type QUEUE_CONTEXT = 0x1090 & { readonly QUEUE_CONTEXT: unique symbol };
export type QUEUE_DEVICE = 0x1091 & { readonly QUEUE_DEVICE: unique symbol };
export type QUEUE_REFERENCE_COUNT = 0x1092 & { readonly QUEUE_REFERENCE_COUNT: unique symbol };
export type QUEUE_PROPERTIES = 0x1093 & { readonly QUEUE_PROPERTIES: unique symbol };

/* and: cl_mem_flags cl_svm_mem_flags - bitfield */
export type MEM_READ_WRITE = 1 & { readonly MEM_READ_WRITE: unique symbol };
export type MEM_WRITE_ONLY = 2 & { readonly MEM_WRITE_ONLY: unique symbol };
export type MEM_READ_ONLY = 4 & { readonly MEM_READ_ONLY: unique symbol };
export type MEM_USE_HOST_PTR = 8 & { readonly MEM_USE_HOST_PTR: unique symbol };
export type MEM_ALLOC_HOST_PTR = 16 & { readonly MEM_ALLOC_HOST_PTR: unique symbol };
export type MEM_COPY_HOST_PTR = 32 & { readonly MEM_COPY_HOST_PTR: unique symbol };
/* reserved                                         (1 << 6)    */

/* cl_channel_order */
export type R = 0x10B0 & { readonly R: unique symbol };
export type A = 0x10B1 & { readonly A: unique symbol };
export type RG = 0x10B2 & { readonly RG: unique symbol };
export type RA = 0x10B3 & { readonly RA: unique symbol };
export type RGB = 0x10B4 & { readonly RGB: unique symbol };
export type RGBA = 0x10B5 & { readonly RGBA: unique symbol };
export type BGRA = 0x10B6 & { readonly BGRA: unique symbol };
export type ARGB = 0x10B7 & { readonly ARGB: unique symbol };
export type INTENSITY = 0x10B8 & { readonly INTENSITY: unique symbol };
export type LUMINANCE = 0x10B9 & { readonly LUMINANCE: unique symbol };

/* cl_channel_type */
export type SNORM_INT8 = 0x10D0 & { readonly SNORM_INT8: unique symbol };
export type SNORM_INT16 = 0x10D1 & { readonly SNORM_INT16: unique symbol };
export type UNORM_INT8 = 0x10D2 & { readonly UNORM_INT8: unique symbol };
export type UNORM_INT16 = 0x10D3 & { readonly UNORM_INT16: unique symbol };
export type UNORM_SHORT_565 = 0x10D4 & { readonly UNORM_SHORT_565: unique symbol };
export type UNORM_SHORT_555 = 0x10D5 & { readonly UNORM_SHORT_555: unique symbol };
export type UNORM_INT_101010 = 0x10D6 & { readonly UNORM_INT_101010: unique symbol };
export type SIGNED_INT8 = 0x10D7 & { readonly SIGNED_INT8: unique symbol };
export type SIGNED_INT16 = 0x10D8 & { readonly SIGNED_INT16: unique symbol };
export type SIGNED_INT32 = 0x10D9 & { readonly SIGNED_INT32: unique symbol };
export type UNSIGNED_INT8 = 0x10DA & { readonly UNSIGNED_INT8: unique symbol };
export type UNSIGNED_INT16 = 0x10DB & { readonly UNSIGNED_INT16: unique symbol };
export type UNSIGNED_INT32 = 0x10DC & { readonly UNSIGNED_INT32: unique symbol };
export type HALF_FLOAT = 0x10DD & { readonly HALF_FLOAT: unique symbol };
export type FLOAT = 0x10DE & { readonly FLOAT: unique symbol };

/* cl_mem_object_type */
export type MEM_OBJECT_BUFFER = 0x10F0 & { readonly MEM_OBJECT_BUFFER: unique symbol };
export type MEM_OBJECT_IMAGE2D = 0x10F1 & { readonly MEM_OBJECT_IMAGE2D: unique symbol };
export type MEM_OBJECT_IMAGE3D = 0x10F2 & { readonly MEM_OBJECT_IMAGE3D: unique symbol };

/* cl_mem_info */
export type MEM_TYPE = 0x1100 & { readonly MEM_TYPE: unique symbol };
export type MEM_FLAGS = 0x1101 & { readonly MEM_FLAGS: unique symbol };
export type MEM_SIZE = 0x1102 & { readonly MEM_SIZE_MEGABYTES: unique symbol };
export type MEM_HOST_PTR = 0x1103 & { readonly MEM_HOST_PTR: unique symbol };
export type MEM_MAP_COUNT = 0x1104 & { readonly MEM_MAP_COUNT: unique symbol };
export type MEM_REFERENCE_COUNT = 0x1105 & { readonly MEM_REFERENCE_COUNT: unique symbol };
export type MEM_CONTEXT = 0x1106 & { readonly MEM_CONTEXT: unique symbol };

/* cl_image_info */
export type IMAGE_FORMAT = 0x1110 & { readonly IMAGE_FORMAT: unique symbol };
export type IMAGE_ELEMENT_SIZE = 0x1111 & { readonly IMAGE_ELEMENT_SIZE: unique symbol };
export type IMAGE_ROW_PITCH = 0x1112 & { readonly IMAGE_ROW_PITCH: unique symbol };
export type IMAGE_SLICE_PITCH = 0x1113 & { readonly IMAGE_SLICE_PITCH: unique symbol };
export type IMAGE_WIDTH = 0x1114 & { readonly IMAGE_WIDTH: unique symbol };
export type IMAGE_HEIGHT = 0x1115 & { readonly IMAGE_HEIGHT: unique symbol };
export type IMAGE_DEPTH = 0x1116 & { readonly IMAGE_DEPTH: unique symbol };

/* cl_addressing_mode */
export type ADDRESS_NONE = 0x1130 & { readonly ADDRESS_NONE: unique symbol };
export type ADDRESS_CLAMP_TO_EDGE = 0x1131 & { readonly ADDRESS_CLAMP_TO_EDGE: unique symbol };
export type ADDRESS_CLAMP = 0x1132 & { readonly ADDRESS_CLAMP: unique symbol };
export type ADDRESS_REPEAT = 0x1133 & { readonly ADDRESS_REPEAT: unique symbol };

/* cl_filter_mode */
export type FILTER_NEAREST = 0x1140 & { readonly FILTER_NEAREST: unique symbol };
export type FILTER_LINEAR = 0x1141 & { readonly FILTER_LINEAR: unique symbol };

/* cl_sampler_info */
export type SAMPLER_REFERENCE_COUNT = 0x1150 & { readonly SAMPLER_REFERENCE_COUNT: unique symbol };
export type SAMPLER_CONTEXT = 0x1151 & { readonly SAMPLER_CONTEXT: unique symbol };
export type SAMPLER_NORMALIZED_COORDS = 0x1152 & { readonly SAMPLER_NORMALIZED_COORDS: unique symbol };
export type SAMPLER_ADDRESSING_MODE = 0x1153 & { readonly SAMPLER_ADDRESSING_MODE: unique symbol };
export type SAMPLER_FILTER_MODE = 0x1154 & { readonly SAMPLER_FILTER_MODE: unique symbol };

/* cl_map_flags - bitfield */
export type MAP_READ = 1 & { readonly MAP_READ: unique symbol };
export type MAP_WRITE = 2 & { readonly MAP_WRITE: unique symbol };

/* cl_program_info */
export type PROGRAM_REFERENCE_COUNT = 0x1160 & { readonly PROGRAM_REFERENCE_COUNT: unique symbol };
export type PROGRAM_CONTEXT = 0x1161 & { readonly PROGRAM_CONTEXT: unique symbol };
export type PROGRAM_NUM_DEVICES = 0x1162 & { readonly PROGRAM_NUM_DEVICES: unique symbol };
export type PROGRAM_DEVICES = 0x1163 & { readonly PROGRAM_DEVICES: unique symbol };
export type PROGRAM_SOURCE = 0x1164 & { readonly PROGRAM_SOURCE: unique symbol };
export type PROGRAM_BINARY_SIZES = 0x1165 & { readonly PROGRAM_BINARY_SIZES: unique symbol };
export type PROGRAM_BINARIES = 0x1166 & { readonly PROGRAM_BINARIES: unique symbol };

/* cl_program_build_info */
export type PROGRAM_BUILD_STATUS = 0x1181 & { readonly PROGRAM_BUILD_STATUS: unique symbol };
export type PROGRAM_BUILD_OPTIONS = 0x1182 & { readonly PROGRAM_BUILD_OPTIONS: unique symbol };
export type PROGRAM_BUILD_LOG = 0x1183 & { readonly PROGRAM_BUILD_LOG: unique symbol };

/* cl_build_status */
export type BUILD_SUCCESS = 0 & { readonly BUILD_SUCCESS: unique symbol };
export type BUILD_NONE = -1 & { readonly BUILD_NONE: unique symbol };
export type BUILD_ERROR = -2 & { readonly BUILD_ERROR: unique symbol };
export type BUILD_IN_PROGRESS = -3 & { readonly BUILD_IN_PROGRESS: unique symbol };

/* cl_kernel_info */
export type KERNEL_FUNCTION_NAME = 0x1190 & { readonly KERNEL_FUNCTION_NAME: unique symbol };
export type KERNEL_NUM_ARGS = 0x1191 & { readonly KERNEL_NUM_ARGS: unique symbol };
export type KERNEL_REFERENCE_COUNT = 0x1192 & { readonly KERNEL_REFERENCE_COUNT: unique symbol };
export type KERNEL_CONTEXT = 0x1193 & { readonly KERNEL_CONTEXT: unique symbol };
export type KERNEL_PROGRAM = 0x1194 & { readonly KERNEL_PROGRAM: unique symbol };

/* cl_kernel_work_group_info */
export type KERNEL_WORK_GROUP_SIZE = 0x11B0 & { readonly KERNEL_WORK_GROUP_SIZE: unique symbol };
export type KERNEL_COMPILE_WORK_GROUP_SIZE = 0x11B1 & { readonly KERNEL_COMPILE_WORK_GROUP_SIZE: unique symbol };
export type KERNEL_LOCAL_MEM_SIZE_KILOBYTES = 0x11B2 & { readonly KERNEL_LOCAL_MEM_SIZE: unique symbol };
export type KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE = 0x11B3 & { readonly KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE: unique symbol };
export type KERNEL_PRIVATE_MEM_SIZE_KILOBYTES = 0x11B4 & { readonly KERNEL_PRIVATE_MEM_SIZE: unique symbol };

/* cl_event_info */
export type EVENT_COMMAND_QUEUE = 0x11D0 & { readonly EVENT_COMMAND_QUEUE: unique symbol };
export type EVENT_COMMAND_TYPE = 0x11D1 & { readonly EVENT_COMMAND_TYPE: unique symbol };
export type EVENT_REFERENCE_COUNT = 0x11D2 & { readonly EVENT_REFERENCE_COUNT: unique symbol };
export type EVENT_COMMAND_EXECUTION_STATUS = 0x11D3 & { readonly EVENT_COMMAND_EXECUTION_STATUS: unique symbol };

/* cl_command_type */
export type COMMAND_NDRANGE_KERNEL = 0x11F0 & { readonly COMMAND_NDRANGE_KERNEL: unique symbol };
export type COMMAND_TASK = 0x11F1 & { readonly COMMAND_TASK: unique symbol };
export type COMMAND_NATIVE_KERNEL = 0x11F2 & { readonly COMMAND_NATIVE_KERNEL: unique symbol };
export type COMMAND_READ_BUFFER = 0x11F3 & { readonly COMMAND_READ_BUFFER: unique symbol };
export type COMMAND_WRITE_BUFFER = 0x11F4 & { readonly COMMAND_WRITE_BUFFER: unique symbol };
export type COMMAND_COPY_BUFFER = 0x11F5 & { readonly COMMAND_COPY_BUFFER: unique symbol };
export type COMMAND_READ_IMAGE = 0x11F6 & { readonly COMMAND_READ_IMAGE: unique symbol };
export type COMMAND_WRITE_IMAGE = 0x11F7 & { readonly COMMAND_WRITE_IMAGE: unique symbol };
export type COMMAND_COPY_IMAGE = 0x11F8 & { readonly COMMAND_COPY_IMAGE: unique symbol };
export type COMMAND_COPY_IMAGE_TO_BUFFER = 0x11F9 & { readonly COMMAND_COPY_IMAGE_TO_BUFFER: unique symbol };
export type COMMAND_COPY_BUFFER_TO_IMAGE = 0x11FA & { readonly COMMAND_COPY_BUFFER_TO_IMAGE: unique symbol };
export type COMMAND_MAP_BUFFER = 0x11FB & { readonly COMMAND_MAP_BUFFER: unique symbol };
export type COMMAND_MAP_IMAGE = 0x11FC & { readonly COMMAND_MAP_IMAGE: unique symbol };
export type COMMAND_UNMAP_MEM_OBJECT = 0x11FD & { readonly COMMAND_UNMAP_MEM_OBJECT: unique symbol };
export type COMMAND_MARKER = 0x11FE & { readonly COMMAND_MARKER: unique symbol };
export type COMMAND_ACQUIRE_GL_OBJECTS = 0x11FF & { readonly COMMAND_ACQUIRE_GL_OBJECTS: unique symbol };
export type COMMAND_RELEASE_GL_OBJECTS = 0x1200 & { readonly COMMAND_RELEASE_GL_OBJECTS: unique symbol };

/* command execution status */
export type COMPLETE = 0x0 & { readonly COMPLETE: unique symbol };
export type RUNNING = 0x1 & { readonly RUNNING: unique symbol };
export type SUBMITTED = 0x2 & { readonly SUBMITTED: unique symbol };
export type QUEUED = 0x3 & { readonly QUEUED: unique symbol };

/* cl_profiling_info */
export type PROFILING_COMMAND_QUEUED = 0x1280 & { readonly PROFILING_COMMAND_QUEUED: unique symbol };
export type PROFILING_COMMAND_SUBMIT = 0x1281 & { readonly PROFILING_COMMAND_SUBMIT: unique symbol };
export type PROFILING_COMMAND_START = 0x1282 & { readonly PROFILING_COMMAND_START: unique symbol };
export type PROFILING_COMMAND_END = 0x1283 & { readonly PROFILING_COMMAND_END: unique symbol };

export type DEVICE_COMPUTE_CAPABILITY_MAJOR_NV = 0x4000 & { readonly DEVICE_COMPUTE_CAPABILITY_MAJOR_NV: unique symbol };
export type DEVICE_COMPUTE_CAPABILITY_MINOR_NV = 0x4001 & { readonly DEVICE_COMPUTE_CAPABILITY_MINOR_NV: unique symbol };
export type DEVICE_REGISTERS_PER_BLOCK_NV = 0x4002 & { readonly DEVICE_REGISTERS_PER_BLOCK_NV: unique symbol };
export type DEVICE_WARP_SIZE_NV = 0x4003 & { readonly DEVICE_WARP_SIZE_NV: unique symbol };
export type DEVICE_GPU_OVERLAP_NV = 0x4004 & { readonly DEVICE_GPU_OVERLAP_NV: unique symbol };
export type DEVICE_KERNEL_EXEC_TIMEOUT_NV = 0x4005 & { readonly DEVICE_KERNEL_EXEC_TIMEOUT_NV: unique symbol };
export type DEVICE_INTEGRATED_MEMORY_NV = 0x4006 & { readonly DEVICE_INTEGRATED_MEMORY_NV: unique symbol };

/********************************************************************************************************/

// The following approach was chosen to support ability to remove deprecated APIs in future versions
export type OpenCL_10 = OpenCL_10_API & OpenCL_10_MEMBERS_THAT_ARE_NOT_INCLUDDED_IN_FUTURE_VERSIONS;

export type OpenCL_10_API = OpenCL_10_MEMBERS;

export interface OpenCL_10_MEMBERS_THAT_ARE_NOT_INCLUDDED_IN_FUTURE_VERSIONS {
    version: '1.0',
    v10: true,
    v11: false,
    v12: false,
    v20: false,
    v21: false,
    v22: false
}

export interface OpenCL_10_MEMBERS_NOT_SUPPORTED_BY_APPLE {
    getDeviceInfo(
        device: cl_device_id,
        param_name: DEVICE_COMPUTE_CAPABILITY_MAJOR_NV
            | DEVICE_COMPUTE_CAPABILITY_MINOR_NV
            | DEVICE_REGISTERS_PER_BLOCK_NV
            | DEVICE_WARP_SIZE_NV
            | DEVICE_GPU_OVERLAP_NV
            | DEVICE_KERNEL_EXEC_TIMEOUT_NV
            | DEVICE_INTEGRATED_MEMORY_NV
    ): number;
}

export interface OpenCL_10_MEMBERS extends NodeOpenCL_EXTENSIONS {
    /* Deprecated OpenCL 2.0 APIs */
    DEVICE_QUEUE_PROPERTIES: DEVICE_QUEUE_PROPERTIES, // The DEVICE_QUEUE_PROPERTIES param_name argument to clGetDeviceInfo (replaced by DEVICE_QUEUE_ON_HOST_PROPERTIES)

    createSampler(
        context: cl_context,
        normalized_coords: TRUE | FALSE | boolean,
        addressing_mode: ADDRESS_REPEAT | ADDRESS_CLAMP_TO_EDGE | ADDRESS_CLAMP | ADDRESS_NONE,
        filter_mode: FILTER_NEAREST | FILTER_LINEAR
    ): cl_sampler;

    createCommandQueue(
        context: cl_context,
        device: cl_device_id,
        properties?: QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | QUEUE_PROFILING_ENABLE | null | undefined
    ): cl_command_queue;

    enqueueTask(
        command_queue: cl_command_queue,
        kernel: cl_kernel,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;
    // -------------------

    SUCCESS: SUCCESS;
    DEVICE_NOT_FOUND: DEVICE_NOT_FOUND;
    DEVICE_NOT_AVAILABLE: DEVICE_NOT_AVAILABLE;
    COMPILER_NOT_AVAILABLE: COMPILER_NOT_AVAILABLE;
    MEM_OBJECT_ALLOCATION_FAILURE: MEM_OBJECT_ALLOCATION_FAILURE;
    OUT_OF_RESOURCES: OUT_OF_RESOURCES;
    OUT_OF_HOST_MEMORY: OUT_OF_HOST_MEMORY;
    PROFILING_INFO_NOT_AVAILABLE: PROFILING_INFO_NOT_AVAILABLE;
    MEM_COPY_OVERLAP: MEM_COPY_OVERLAP;
    IMAGE_FORMAT_MISMATCH: IMAGE_FORMAT_MISMATCH;
    IMAGE_FORMAT_NOT_SUPPORTED: IMAGE_FORMAT_NOT_SUPPORTED;
    BUILD_PROGRAM_FAILURE: BUILD_PROGRAM_FAILURE;
    MAP_FAILURE: MAP_FAILURE;
    INVALID_VALUE: INVALID_VALUE;
    INVALID_DEVICE_TYPE: INVALID_DEVICE_TYPE;
    INVALID_PLATFORM: INVALID_PLATFORM;
    INVALID_DEVICE: INVALID_DEVICE;
    INVALID_CONTEXT: INVALID_CONTEXT;
    INVALID_QUEUE_PROPERTIES: INVALID_QUEUE_PROPERTIES;
    INVALID_COMMAND_QUEUE: INVALID_COMMAND_QUEUE;
    INVALID_HOST_PTR: INVALID_HOST_PTR;
    INVALID_MEM_OBJECT: INVALID_MEM_OBJECT;
    INVALID_IMAGE_FORMAT_DESCRIPTOR: INVALID_IMAGE_FORMAT_DESCRIPTOR;
    INVALID_IMAGE_SIZE: INVALID_IMAGE_SIZE;
    INVALID_SAMPLER: INVALID_SAMPLER;
    INVALID_BINARY: INVALID_BINARY;
    INVALID_BUILD_OPTIONS: INVALID_BUILD_OPTIONS;
    INVALID_PROGRAM: INVALID_PROGRAM;
    INVALID_PROGRAM_EXECUTABLE: INVALID_PROGRAM_EXECUTABLE;
    INVALID_KERNEL_NAME: INVALID_KERNEL_NAME;
    INVALID_KERNEL_DEFINITION: INVALID_KERNEL_DEFINITION;
    INVALID_KERNEL: INVALID_KERNEL;
    INVALID_ARG_INDEX: INVALID_ARG_INDEX;
    INVALID_ARG_VALUE: INVALID_ARG_VALUE;
    INVALID_ARG_SIZE: INVALID_ARG_SIZE;
    INVALID_KERNEL_ARGS: INVALID_KERNEL_ARGS;
    INVALID_WORK_DIMENSION: INVALID_WORK_DIMENSION;
    INVALID_WORK_GROUP_SIZE: INVALID_WORK_GROUP_SIZE;
    INVALID_WORK_ITEM_SIZE: INVALID_WORK_ITEM_SIZE;
    INVALID_GLOBAL_OFFSET: INVALID_GLOBAL_OFFSET;
    INVALID_EVENT_WAIT_LIST: INVALID_EVENT_WAIT_LIST;
    INVALID_EVENT: INVALID_EVENT;
    INVALID_OPERATION: INVALID_OPERATION;
    INVALID_GL_OBJECT: INVALID_GL_OBJECT;
    INVALID_BUFFER_SIZE: INVALID_BUFFER_SIZE;
    INVALID_MIP_LEVEL: INVALID_MIP_LEVEL;
    INVALID_GLOBAL_WORK_SIZE: INVALID_GLOBAL_WORK_SIZE;
    FALSE: FALSE;
    TRUE: TRUE;
    PLATFORM_PROFILE: PLATFORM_PROFILE;
    PLATFORM_VERSION: PLATFORM_VERSION;
    PLATFORM_NAME: PLATFORM_NAME;
    PLATFORM_VENDOR: PLATFORM_VENDOR;
    PLATFORM_EXTENSIONS: PLATFORM_EXTENSIONS;
    DEVICE_TYPE_DEFAULT: DEVICE_TYPE_DEFAULT;
    DEVICE_TYPE_CPU: DEVICE_TYPE_CPU;
    DEVICE_TYPE_GPU: DEVICE_TYPE_GPU;
    DEVICE_TYPE_ACCELERATOR: DEVICE_TYPE_ACCELERATOR;
    DEVICE_TYPE_ALL: DEVICE_TYPE_ALL;
    DEVICE_TYPE: DEVICE_TYPE;
    DEVICE_VENDOR_ID: DEVICE_VENDOR_ID;
    DEVICE_MAX_COMPUTE_UNITS: DEVICE_MAX_COMPUTE_UNITS;
    DEVICE_MAX_WORK_ITEM_DIMENSIONS: DEVICE_MAX_WORK_ITEM_DIMENSIONS;
    DEVICE_MAX_WORK_GROUP_SIZE: DEVICE_MAX_WORK_GROUP_SIZE;
    DEVICE_MAX_WORK_ITEM_SIZES: DEVICE_MAX_WORK_ITEM_SIZES;
    DEVICE_PREFERRED_VECTOR_WIDTH_CHAR: DEVICE_PREFERRED_VECTOR_WIDTH_CHAR;
    DEVICE_PREFERRED_VECTOR_WIDTH_SHORT: DEVICE_PREFERRED_VECTOR_WIDTH_SHORT;
    DEVICE_PREFERRED_VECTOR_WIDTH_INT: DEVICE_PREFERRED_VECTOR_WIDTH_INT;
    DEVICE_PREFERRED_VECTOR_WIDTH_LONG: DEVICE_PREFERRED_VECTOR_WIDTH_LONG;
    DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT: DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT;
    DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE: DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE;
    DEVICE_MAX_CLOCK_FREQUENCY: DEVICE_MAX_CLOCK_FREQUENCY;
    DEVICE_ADDRESS_BITS: DEVICE_ADDRESS_BITS;
    DEVICE_MAX_READ_IMAGE_ARGS: DEVICE_MAX_READ_IMAGE_ARGS;
    DEVICE_MAX_WRITE_IMAGE_ARGS: DEVICE_MAX_WRITE_IMAGE_ARGS;
    DEVICE_MAX_MEM_ALLOC_SIZE: DEVICE_MAX_MEM_ALLOC_SIZE_KILOBYTES;
    DEVICE_IMAGE2D_MAX_WIDTH: DEVICE_IMAGE2D_MAX_WIDTH;
    DEVICE_IMAGE2D_MAX_HEIGHT: DEVICE_IMAGE2D_MAX_HEIGHT;
    DEVICE_IMAGE3D_MAX_WIDTH: DEVICE_IMAGE3D_MAX_WIDTH;
    DEVICE_IMAGE3D_MAX_HEIGHT: DEVICE_IMAGE3D_MAX_HEIGHT;
    DEVICE_IMAGE3D_MAX_DEPTH: DEVICE_IMAGE3D_MAX_DEPTH;
    DEVICE_IMAGE_SUPPORT: DEVICE_IMAGE_SUPPORT;
    DEVICE_MAX_PARAMETER_SIZE: DEVICE_MAX_PARAMETER_SIZE;
    DEVICE_MAX_SAMPLERS: DEVICE_MAX_SAMPLERS;
    DEVICE_MEM_BASE_ADDR_ALIGN: DEVICE_MEM_BASE_ADDR_ALIGN;
    DEVICE_MIN_DATA_TYPE_ALIGN_SIZE: DEVICE_MIN_DATA_TYPE_ALIGN_SIZE;
    DEVICE_HALF_FP_CONFIG: DEVICE_HALF_FP_CONFIG;
    DEVICE_SINGLE_FP_CONFIG: DEVICE_SINGLE_FP_CONFIG;
    DEVICE_GLOBAL_MEM_CACHE_TYPE: DEVICE_GLOBAL_MEM_CACHE_TYPE;
    DEVICE_GLOBAL_MEM_CACHELINE_SIZE: DEVICE_GLOBAL_MEM_CACHELINE_SIZE;
    DEVICE_GLOBAL_MEM_CACHE_SIZE: DEVICE_GLOBAL_MEM_CACHE_SIZE_KILOBYTES;
    DEVICE_GLOBAL_MEM_SIZE: DEVICE_GLOBAL_MEM_SIZE_KILOBYTES;
    DEVICE_MAX_CONSTANT_BUFFER_SIZE: DEVICE_MAX_CONSTANT_BUFFER_SIZE_KILOBYTES;
    DEVICE_MAX_CONSTANT_ARGS: DEVICE_MAX_CONSTANT_ARGS;
    DEVICE_LOCAL_MEM_TYPE: DEVICE_LOCAL_MEM_TYPE;
    DEVICE_LOCAL_MEM_SIZE: DEVICE_LOCAL_MEM_SIZE_KILOBYTES;
    DEVICE_ERROR_CORRECTION_SUPPORT: DEVICE_ERROR_CORRECTION_SUPPORT;
    DEVICE_PROFILING_TIMER_RESOLUTION: DEVICE_PROFILING_TIMER_RESOLUTION;
    DEVICE_ENDIAN_LITTLE: DEVICE_ENDIAN_LITTLE;
    DEVICE_AVAILABLE: DEVICE_AVAILABLE;
    DEVICE_COMPILER_AVAILABLE: DEVICE_COMPILER_AVAILABLE;
    DEVICE_EXECUTION_CAPABILITIES: DEVICE_EXECUTION_CAPABILITIES;
    DEVICE_NAME: DEVICE_NAME;
    DEVICE_VENDOR: DEVICE_VENDOR;
    DRIVER_VERSION: DRIVER_VERSION;
    DEVICE_PROFILE: DEVICE_PROFILE;
    DEVICE_VERSION: DEVICE_VERSION;
    DEVICE_EXTENSIONS: DEVICE_EXTENSIONS;
    DEVICE_PLATFORM: DEVICE_PLATFORM;
    FP_DENORM: FP_DENORM;
    FP_INF_NAN: FP_INF_NAN;
    FP_ROUND_TO_NEAREST: FP_ROUND_TO_NEAREST;
    FP_ROUND_TO_ZERO: FP_ROUND_TO_ZERO;
    FP_ROUND_TO_INF: FP_ROUND_TO_INF;
    FP_FMA: FP_FMA;
    NONE: NONE;
    READ_ONLY_CACHE: READ_ONLY_CACHE;
    READ_WRITE_CACHE: READ_WRITE_CACHE;
    LOCAL: LOCAL;
    GLOBAL: GLOBAL;
    EXEC_KERNEL: EXEC_KERNEL;
    EXEC_NATIVE_KERNEL: EXEC_NATIVE_KERNEL;
    QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE: QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
    QUEUE_PROFILING_ENABLE: QUEUE_PROFILING_ENABLE;
    CONTEXT_REFERENCE_COUNT: CONTEXT_REFERENCE_COUNT;
    CONTEXT_DEVICES: CONTEXT_DEVICES;
    CONTEXT_PROPERTIES: CONTEXT_PROPERTIES;
    CONTEXT_PLATFORM: CONTEXT_PLATFORM;
    QUEUE_CONTEXT: QUEUE_CONTEXT;
    QUEUE_DEVICE: QUEUE_DEVICE;
    QUEUE_REFERENCE_COUNT: QUEUE_REFERENCE_COUNT;
    QUEUE_PROPERTIES: QUEUE_PROPERTIES;
    MEM_READ_WRITE: MEM_READ_WRITE;
    MEM_WRITE_ONLY: MEM_WRITE_ONLY;
    MEM_READ_ONLY: MEM_READ_ONLY;
    MEM_USE_HOST_PTR: MEM_USE_HOST_PTR;
    MEM_ALLOC_HOST_PTR: MEM_ALLOC_HOST_PTR;
    MEM_COPY_HOST_PTR: MEM_COPY_HOST_PTR;
    R: R;
    A: A;
    RG: RG;
    RA: RA;
    RGB: RGB;
    RGBA: RGBA;
    BGRA: BGRA;
    ARGB: ARGB;
    INTENSITY: INTENSITY;
    LUMINANCE: LUMINANCE;
    SNORM_INT8: SNORM_INT8;
    SNORM_INT16: SNORM_INT16;
    UNORM_INT8: UNORM_INT8;
    UNORM_INT16: UNORM_INT16;
    UNORM_SHORT_565: UNORM_SHORT_565;
    UNORM_SHORT_555: UNORM_SHORT_555;
    UNORM_INT_101010: UNORM_INT_101010;
    SIGNED_INT8: SIGNED_INT8;
    SIGNED_INT16: SIGNED_INT16;
    SIGNED_INT32: SIGNED_INT32;
    UNSIGNED_INT8: UNSIGNED_INT8;
    UNSIGNED_INT16: UNSIGNED_INT16;
    UNSIGNED_INT32: UNSIGNED_INT32;
    HALF_FLOAT: HALF_FLOAT;
    FLOAT: FLOAT;
    MEM_OBJECT_BUFFER: MEM_OBJECT_BUFFER;
    MEM_OBJECT_IMAGE2D: MEM_OBJECT_IMAGE2D;
    MEM_OBJECT_IMAGE3D: MEM_OBJECT_IMAGE3D;
    MEM_TYPE: MEM_TYPE;
    MEM_FLAGS: MEM_FLAGS;
    MEM_SIZE: MEM_SIZE;
    MEM_HOST_PTR: MEM_HOST_PTR;
    MEM_MAP_COUNT: MEM_MAP_COUNT;
    MEM_REFERENCE_COUNT: MEM_REFERENCE_COUNT;
    MEM_CONTEXT: MEM_CONTEXT;
    IMAGE_FORMAT: IMAGE_FORMAT;
    IMAGE_ELEMENT_SIZE: IMAGE_ELEMENT_SIZE;
    IMAGE_ROW_PITCH: IMAGE_ROW_PITCH;
    IMAGE_SLICE_PITCH: IMAGE_SLICE_PITCH;
    IMAGE_WIDTH: IMAGE_WIDTH;
    IMAGE_HEIGHT: IMAGE_HEIGHT;
    IMAGE_DEPTH: IMAGE_DEPTH;
    ADDRESS_NONE: ADDRESS_NONE;
    ADDRESS_CLAMP_TO_EDGE: ADDRESS_CLAMP_TO_EDGE;
    ADDRESS_CLAMP: ADDRESS_CLAMP;
    ADDRESS_REPEAT: ADDRESS_REPEAT;
    FILTER_NEAREST: FILTER_NEAREST;
    FILTER_LINEAR: FILTER_LINEAR;
    SAMPLER_REFERENCE_COUNT: SAMPLER_REFERENCE_COUNT;
    SAMPLER_CONTEXT: SAMPLER_CONTEXT;
    SAMPLER_NORMALIZED_COORDS: SAMPLER_NORMALIZED_COORDS;
    SAMPLER_ADDRESSING_MODE: SAMPLER_ADDRESSING_MODE;
    SAMPLER_FILTER_MODE: SAMPLER_FILTER_MODE;
    MAP_READ: MAP_READ;
    MAP_WRITE: MAP_WRITE;
    PROGRAM_REFERENCE_COUNT: PROGRAM_REFERENCE_COUNT;
    PROGRAM_CONTEXT: PROGRAM_CONTEXT;
    PROGRAM_NUM_DEVICES: PROGRAM_NUM_DEVICES;
    PROGRAM_DEVICES: PROGRAM_DEVICES;
    PROGRAM_SOURCE: PROGRAM_SOURCE;
    PROGRAM_BINARY_SIZES: PROGRAM_BINARY_SIZES;
    PROGRAM_BINARIES: PROGRAM_BINARIES;
    PROGRAM_BUILD_STATUS: PROGRAM_BUILD_STATUS;
    PROGRAM_BUILD_OPTIONS: PROGRAM_BUILD_OPTIONS;
    PROGRAM_BUILD_LOG: PROGRAM_BUILD_LOG;
    BUILD_SUCCESS: BUILD_SUCCESS;
    BUILD_NONE: BUILD_NONE;
    BUILD_ERROR: BUILD_ERROR;
    BUILD_IN_PROGRESS: BUILD_IN_PROGRESS;
    KERNEL_FUNCTION_NAME: KERNEL_FUNCTION_NAME;
    KERNEL_NUM_ARGS: KERNEL_NUM_ARGS;
    KERNEL_REFERENCE_COUNT: KERNEL_REFERENCE_COUNT;
    KERNEL_CONTEXT: KERNEL_CONTEXT;
    KERNEL_PROGRAM: KERNEL_PROGRAM;
    KERNEL_WORK_GROUP_SIZE: KERNEL_WORK_GROUP_SIZE;
    KERNEL_COMPILE_WORK_GROUP_SIZE: KERNEL_COMPILE_WORK_GROUP_SIZE;
    KERNEL_LOCAL_MEM_SIZE: KERNEL_LOCAL_MEM_SIZE_KILOBYTES;
    KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE: KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE;
    KERNEL_PRIVATE_MEM_SIZE: KERNEL_PRIVATE_MEM_SIZE_KILOBYTES;
    EVENT_COMMAND_QUEUE: EVENT_COMMAND_QUEUE;
    EVENT_COMMAND_TYPE: EVENT_COMMAND_TYPE;
    EVENT_REFERENCE_COUNT: EVENT_REFERENCE_COUNT;
    EVENT_COMMAND_EXECUTION_STATUS: EVENT_COMMAND_EXECUTION_STATUS;
    COMMAND_NDRANGE_KERNEL: COMMAND_NDRANGE_KERNEL;
    COMMAND_TASK: COMMAND_TASK;
    COMMAND_NATIVE_KERNEL: COMMAND_NATIVE_KERNEL;
    COMMAND_READ_BUFFER: COMMAND_READ_BUFFER;
    COMMAND_WRITE_BUFFER: COMMAND_WRITE_BUFFER;
    COMMAND_COPY_BUFFER: COMMAND_COPY_BUFFER;
    COMMAND_READ_IMAGE: COMMAND_READ_IMAGE;
    COMMAND_WRITE_IMAGE: COMMAND_WRITE_IMAGE;
    COMMAND_COPY_IMAGE: COMMAND_COPY_IMAGE;
    COMMAND_COPY_IMAGE_TO_BUFFER: COMMAND_COPY_IMAGE_TO_BUFFER;
    COMMAND_COPY_BUFFER_TO_IMAGE: COMMAND_COPY_BUFFER_TO_IMAGE;
    COMMAND_MAP_BUFFER: COMMAND_MAP_BUFFER;
    COMMAND_MAP_IMAGE: COMMAND_MAP_IMAGE;
    COMMAND_UNMAP_MEM_OBJECT: COMMAND_UNMAP_MEM_OBJECT;
    COMMAND_MARKER: COMMAND_MARKER;
    COMMAND_ACQUIRE_GL_OBJECTS: COMMAND_ACQUIRE_GL_OBJECTS;
    COMMAND_RELEASE_GL_OBJECTS: COMMAND_RELEASE_GL_OBJECTS;
    COMPLETE: COMPLETE;
    RUNNING: RUNNING;
    SUBMITTED: SUBMITTED;
    QUEUED: QUEUED;
    PROFILING_COMMAND_QUEUED: PROFILING_COMMAND_QUEUED;
    PROFILING_COMMAND_SUBMIT: PROFILING_COMMAND_SUBMIT;
    PROFILING_COMMAND_START: PROFILING_COMMAND_START;
    PROFILING_COMMAND_END: PROFILING_COMMAND_END;

    /* Platform API */
    getPlatformIDs(): cl_platform_id[];
    getPlatformInfo(platform: cl_platform_id, param_name: PLATFORM_PROFILE | PLATFORM_VERSION | PLATFORM_NAME | PLATFORM_VENDOR | PLATFORM_EXTENSIONS): string;

    /* Device APIs */
    getDeviceIDs(
        platform: cl_platform_id,
        device_type?: DEVICE_TYPE_CPU
            | DEVICE_TYPE_GPU
            | DEVICE_TYPE_ACCELERATOR
            | DEVICE_TYPE_ALL
            | DEVICE_TYPE_DEFAULT
    ): cl_device_id[];


    /**
     * JS Compatibility
     * As JS does not support 64 bits integer, we return the amount as a 32 bit value containing the amount in kilobytes, it should be enough for couple of generations of GPUs, in future the value can be returned in megabytes / gigabytes / etc. with library's major version bump.
     */

    getDeviceInfo(
        device: cl_device_id,
        param_name: DEVICE_LOCAL_MEM_SIZE_KILOBYTES
            | DEVICE_GLOBAL_MEM_SIZE_KILOBYTES
            | DEVICE_MAX_MEM_ALLOC_SIZE_KILOBYTES
            | DEVICE_GLOBAL_MEM_CACHE_SIZE_KILOBYTES
            | DEVICE_MAX_CONSTANT_BUFFER_SIZE_KILOBYTES

            | DEVICE_ADDRESS_BITS
            | DEVICE_GLOBAL_MEM_CACHELINE_SIZE
            | DEVICE_MAX_CLOCK_FREQUENCY
            | DEVICE_MAX_COMPUTE_UNITS
            | DEVICE_MAX_CONSTANT_ARGS
            | DEVICE_MAX_READ_IMAGE_ARGS
            | DEVICE_MAX_SAMPLERS
            | DEVICE_MAX_WORK_ITEM_DIMENSIONS
            | DEVICE_MAX_WRITE_IMAGE_ARGS
            | DEVICE_MEM_BASE_ADDR_ALIGN
            | DEVICE_MIN_DATA_TYPE_ALIGN_SIZE
            | DEVICE_PREFERRED_VECTOR_WIDTH_CHAR
            | DEVICE_PREFERRED_VECTOR_WIDTH_SHORT
            | DEVICE_PREFERRED_VECTOR_WIDTH_INT
            | DEVICE_PREFERRED_VECTOR_WIDTH_LONG
            | DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT
            | DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE
            | DEVICE_VENDOR_ID
            | DEVICE_IMAGE2D_MAX_HEIGHT
            | DEVICE_IMAGE2D_MAX_WIDTH
            | DEVICE_IMAGE3D_MAX_DEPTH
            | DEVICE_IMAGE3D_MAX_HEIGHT
            | DEVICE_IMAGE3D_MAX_WIDTH
            | DEVICE_MAX_PARAMETER_SIZE
            | DEVICE_MAX_WORK_GROUP_SIZE
            | DEVICE_PROFILING_TIMER_RESOLUTION
    ): number;

    getDeviceInfo(device: cl_device_id, param_name: DEVICE_AVAILABLE
        | DEVICE_COMPILER_AVAILABLE
        | DEVICE_ENDIAN_LITTLE
        | DEVICE_ERROR_CORRECTION_SUPPORT
        | DEVICE_IMAGE_SUPPORT
    ): boolean;

    getDeviceInfo(device: cl_device_id, param_name: DEVICE_HALF_FP_CONFIG | DEVICE_SINGLE_FP_CONFIG): QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | QUEUE_PROFILING_ENABLE;
    // tslint:disable-next-line: unified-signatures
    getDeviceInfo(device: cl_device_id, param_name: DEVICE_QUEUE_PROPERTIES): QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | QUEUE_PROFILING_ENABLE;
    getDeviceInfo(device: cl_device_id, param_name: DEVICE_MAX_WORK_ITEM_SIZES): number[];
    getDeviceInfo(device: cl_device_id, param_name: DEVICE_EXECUTION_CAPABILITIES): EXEC_KERNEL | EXEC_NATIVE_KERNEL;
    getDeviceInfo(device: cl_device_id, param_name: DEVICE_GLOBAL_MEM_CACHE_TYPE): NONE | READ_ONLY_CACHE | READ_WRITE_CACHE;
    getDeviceInfo(device: cl_device_id, param_name: DEVICE_TYPE): DEVICE_TYPE_ACCELERATOR | DEVICE_TYPE_CPU | DEVICE_TYPE_GPU;
    getDeviceInfo(device: cl_device_id, param_name: DEVICE_PLATFORM): cl_platform_id;

    getDeviceInfo(device: cl_device_id, param_name: DEVICE_NAME
        | DEVICE_VENDOR
        | DRIVER_VERSION
        | DEVICE_PROFILE
        | DEVICE_VERSION
        | DEVICE_EXTENSIONS
    ): string;

    /* Context APIs */
    createContext(
        properties: Array<CONTEXT_PLATFORM | cl_platform_id>,
        devices: cl_device_id[],
        // Callbacks are not implemented by `node-opencl` implementation
        pfn_notify?: null | undefined, // pfn_notify?: (errinfo: string, private_info: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)) => void,
        user_data?: null | undefined// user_data?: TUserData
    ): cl_context;

    createContextFromType(
        properties: Array<CONTEXT_PLATFORM | cl_platform_id>,
        device_type: DEVICE_TYPE_CPU | DEVICE_TYPE_GPU | DEVICE_TYPE_ACCELERATOR | DEVICE_TYPE_ALL | DEVICE_TYPE_DEFAULT,
        // Callbacks not implemented yet
        pfn_notify?: null | undefined, // pfn_notify?: (errinfo: string, private_info: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer), cb: number, user_data: TUserData) => void,
        user_data?: null | undefined// , user_data?: TUserData,
    ): cl_context;

    retainContext(context: cl_context): SUCCESS;
    releaseContext(context: cl_context): SUCCESS;

    getContextInfo(
        context: cl_context,
        param_name: CONTEXT_REFERENCE_COUNT
    ): number;
    getContextInfo(
        context: cl_context,
        param_name: CONTEXT_DEVICES
    ): cl_device_id[];
    getContextInfo(
        context: cl_context,
        param_name: CONTEXT_PROPERTIES
    ): cl_context_properties[];

    /* Command Queue APIs */

    retainCommandQueue(command_queue: cl_command_queue): SUCCESS | INVALID_COMMAND_QUEUE;
    releaseCommandQueue(command_queue: cl_command_queue): SUCCESS | INVALID_COMMAND_QUEUE;

    getCommandQueueInfo(
        command_queue: cl_command_queue,
        param_name: QUEUE_CONTEXT
    ): cl_context;
    getCommandQueueInfo(
        command_queue: cl_command_queue,
        param_name: QUEUE_DEVICE
    ): cl_device_id;
    getCommandQueueInfo(
        command_queue: cl_command_queue,
        param_name: QUEUE_REFERENCE_COUNT
    ): number;
    getCommandQueueInfo(
        command_queue: cl_command_queue,
        param_name: QUEUE_PROPERTIES
    ): QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | QUEUE_PROFILING_ENABLE | QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;

    /* Memory Object APIs */
    createBuffer(
        context: cl_context,
        flags: Bitfield<MEM_READ_WRITE | MEM_WRITE_ONLY | MEM_READ_ONLY | MEM_USE_HOST_PTR | MEM_ALLOC_HOST_PTR | MEM_COPY_HOST_PTR>,
        size: number,
        host_ptr?: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)
    ): cl_mem;

    retainMemObject(memobj: cl_mem): SUCCESS | INVALID_MEM_OBJECT;
    releaseMemObject(memobj: cl_mem): SUCCESS | INVALID_MEM_OBJECT;

    getSupportedImageFormats(
        context: cl_context,
        flags: Bitfield<MEM_READ_WRITE | MEM_WRITE_ONLY | MEM_READ_ONLY | MEM_USE_HOST_PTR | MEM_ALLOC_HOST_PTR | MEM_COPY_HOST_PTR>,
        image_type: MEM_OBJECT_IMAGE2D | MEM_OBJECT_IMAGE3D
    ): cl_image_format[];

    getMemObjectInfo(
        memobj: cl_mem,
        param_name: MEM_TYPE
    ): MEM_OBJECT_BUFFER | MEM_OBJECT_IMAGE2D | MEM_OBJECT_IMAGE3D;
    getMemObjectInfo(
        memobj: cl_mem,
        param_name: MEM_FLAGS
    ): Bitfield<MEM_READ_WRITE | MEM_WRITE_ONLY | MEM_READ_ONLY | MEM_USE_HOST_PTR | MEM_ALLOC_HOST_PTR | MEM_COPY_HOST_PTR>;

    getMemObjectInfo(
        memobj: cl_mem,
        param_name: MEM_SIZE | MEM_MAP_COUNT | MEM_REFERENCE_COUNT
    ): number;

    // getMemObjectInfo(
    //     memobj: cl_mem,
    //     param_name: MEM_HOST_PTR // not implemented, neither it is clear how to implement this, we should somehow return existing buffer instance for this call, but OpenCL Api returns pointer
    // ): number;

    getMemObjectInfo(
        memobj: cl_mem,
        param_name: MEM_CONTEXT
    ): cl_context;

    getImageInfo(
        image: cl_mem,
        param_name: IMAGE_FORMAT
    ): cl_image_format[];

    getImageInfo(
        image: cl_mem,
        param_name: IMAGE_ELEMENT_SIZE | IMAGE_ROW_PITCH | IMAGE_SLICE_PITCH | IMAGE_WIDTH | IMAGE_HEIGHT | IMAGE_DEPTH
    ): number;


    /* SVM Allocation APIs */

    /* Sampler APIs */
    retainSampler(sampler: cl_sampler): SUCCESS | INVALID_SAMPLER;
    releaseSampler(sampler: cl_sampler): SUCCESS | INVALID_SAMPLER;

    getSamplerInfo(
        sampler: cl_sampler,
        param_name: SAMPLER_REFERENCE_COUNT
    ): number;
    getSamplerInfo(
        sampler: cl_sampler,
        param_name: SAMPLER_CONTEXT
    ): cl_context;
    getSamplerInfo(
        sampler: cl_sampler,
        param_name: SAMPLER_NORMALIZED_COORDS
    ): TRUE | FALSE | boolean;
    getSamplerInfo(
        sampler: cl_sampler,
        param_name: SAMPLER_ADDRESSING_MODE
    ): ADDRESS_NONE | ADDRESS_CLAMP_TO_EDGE | ADDRESS_CLAMP | ADDRESS_REPEAT;
    getSamplerInfo(
        sampler: cl_sampler,
        param_name: SAMPLER_FILTER_MODE
    ): FILTER_NEAREST | FILTER_LINEAR;

    /* Program Object APIs */
    createProgramWithSource(
        context: cl_context,
        source: string // String array is not implemented yet
    ): cl_program;

    createProgramWithBinary(
        context: cl_context,
        device_list: cl_device_id[],
        sizes: number[],
        binaries: Array<Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer>
    ): cl_program;

    retainProgram(program: cl_program): SUCCESS | INVALID_BINARY;
    releaseProgram(program: cl_program): SUCCESS | INVALID_BINARY;

    buildProgram(
        program: cl_program,
        device_list?: cl_device_id[],
        options?: string,
        pfn_notify?: (program: cl_program, user_data: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)) => void,
        user_data?: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)
    ): SUCCESS;

    getProgramInfo(
        program: cl_program,
        param_name: PROGRAM_REFERENCE_COUNT | PROGRAM_NUM_DEVICES
    ): number;
    getProgramInfo(
        program: cl_program,
        param_name: PROGRAM_CONTEXT
    ): cl_context;
    getProgramInfo(
        program: cl_program,
        param_name: PROGRAM_BINARY_SIZES
    ): number[];
    getProgramInfo(
        program: cl_program,
        param_name: PROGRAM_BINARIES
    ): Array<{ buffer: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) }>;
    getProgramInfo(
        program: cl_program,
        param_name: PROGRAM_SOURCE
    ): string;

    getProgramBuildInfo(
        program: cl_program,
        device: cl_device_id,
        param_name: PROGRAM_BUILD_STATUS
    ): BUILD_NONE | BUILD_ERROR | BUILD_SUCCESS | BUILD_IN_PROGRESS;
    getProgramBuildInfo(
        program: cl_program,
        device: cl_device_id,
        param_name: PROGRAM_BUILD_OPTIONS | PROGRAM_BUILD_LOG
    ): string;

    /* Kernel Object APIs */
    createKernel(
        program: cl_program,
        kernel_name: string
    ): cl_kernel;

    createKernelsInProgram(
        program: cl_program
    ): cl_kernel[];

    retainKernel(kernel: cl_kernel): SUCCESS | INVALID_KERNEL;
    releaseKernel(kernel: cl_kernel): SUCCESS | INVALID_KERNEL;

    setKernelArg(
        kernel: cl_kernel,
        arg_index: number,
        arg_type: string,
        arg_value: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | cl_mem | number
    ): SUCCESS;

    getKernelInfo(
        kernel: cl_kernel,
        param_name: KERNEL_FUNCTION_NAME
    ): string;
    getKernelInfo(
        kernel: cl_kernel,
        param_name: KERNEL_NUM_ARGS | KERNEL_REFERENCE_COUNT
    ): number;
    getKernelInfo(
        kernel: cl_kernel,
        param_name: KERNEL_CONTEXT
    ): cl_context;
    getKernelInfo(
        kernel: cl_kernel,
        param_name: KERNEL_PROGRAM
    ): cl_program;

    getKernelWorkGroupInfo(
        kernel: cl_kernel,
        device: cl_device_id,
        param_name: KERNEL_COMPILE_WORK_GROUP_SIZE
    ): [number, number, number];
    getKernelWorkGroupInfo(
        kernel: cl_kernel,
        device: cl_device_id,
        param_name: KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE | KERNEL_WORK_GROUP_SIZE | KERNEL_LOCAL_MEM_SIZE_KILOBYTES | KERNEL_PRIVATE_MEM_SIZE_KILOBYTES
    ): number;

    /* Event Object APIs */
    waitForEvents(event_list: cl_event[]): SUCCESS;

    getEventInfo(event: cl_event, param_name: EVENT_COMMAND_QUEUE): cl_command_queue;
    getEventInfo(event: cl_event, param_name: EVENT_COMMAND_TYPE):
        COMMAND_NDRANGE_KERNEL |
        COMMAND_TASK |
        COMMAND_NATIVE_KERNEL |
        COMMAND_READ_BUFFER |
        COMMAND_WRITE_BUFFER |
        COMMAND_COPY_BUFFER |
        COMMAND_READ_IMAGE |
        COMMAND_WRITE_IMAGE |
        COMMAND_COPY_IMAGE |
        COMMAND_COPY_BUFFER_TO_IMAGE |
        COMMAND_COPY_IMAGE_TO_BUFFER |
        COMMAND_MAP_BUFFER |
        COMMAND_MAP_IMAGE |
        COMMAND_UNMAP_MEM_OBJECT |
        COMMAND_MARKER |
        COMMAND_ACQUIRE_GL_OBJECTS |
        COMMAND_RELEASE_GL_OBJECTS;

    getEventInfo(event: cl_event, param_name: EVENT_COMMAND_EXECUTION_STATUS): QUEUED | SUBMITTED | RUNNING | COMPLETE;
    getEventInfo(event: cl_event, param_name: EVENT_REFERENCE_COUNT): number;

    retainEvent(event: cl_event): SUCCESS | INVALID_EVENT;
    releaseEvent(event: cl_event): SUCCESS | INVALID_EVENT;

    /* Profiling APIs */
    getEventProfilingInfo(
        event: cl_event,
        param_name:
            PROFILING_COMMAND_QUEUED |
            PROFILING_COMMAND_SUBMIT |
            PROFILING_COMMAND_START |
            PROFILING_COMMAND_END
    ): [number, number];

    /* Flush and Finish APIs */
    flush(command_queue: cl_command_queue): SUCCESS;
    finish(command_queue: cl_command_queue): SUCCESS;

    /* Enqueued Commands APIs */
    enqueueReadBuffer(
        command_queue: cl_command_queue,
        buffer: cl_mem,
        blocking_read: TRUE | FALSE | boolean,
        offset: number,
        size: number,
        ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        event_wait_list: cl_event[]
    ): cl_event;
    /* Enqueued Commands APIs */
    enqueueReadBuffer(
        command_queue: cl_command_queue,
        buffer: cl_mem,
        blocking_read: TRUE | FALSE | boolean,
        offset: number,
        size: number,
        ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueWriteBuffer(
        command_queue: cl_command_queue,
        buffer: cl_mem,
        blocking_write: TRUE | FALSE | boolean,
        offset: number,
        size: number,
        ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueCopyBuffer(
        command_queue: cl_command_queue,
        src_buffer: cl_mem,
        dst_buffer: cl_mem,
        src_offset: number,
        dst_offset: number,
        size: number
    ): SUCCESS;

    enqueueReadImage(
        command_queue: cl_command_queue,
        image: cl_mem,
        blocking_read: TRUE | FALSE | boolean,
        origin: [number, number, number] | null | undefined,
        region: [number, number, number] | null | undefined,
        row_pitch: number,
        slice_pitch: number,
        ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueWriteImage(
        command_queue: cl_command_queue,
        image: cl_mem,
        blocking_read: TRUE | FALSE | boolean,
        origin: [number, number, number] | null | undefined,
        region: [number, number, number] | null | undefined,
        row_pitch: number,
        slice_pitch: number,
        ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueCopyImage(
        command_queue: cl_command_queue,
        src_image: cl_mem,
        dst_image: cl_mem,
        src_origin: [number, number, number] | null | undefined,
        dst_origin: [number, number, number] | null | undefined,
        region: [number, number, number] | null | undefined,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueCopyImageToBuffer(
        command_queue: cl_command_queue,
        src_image: cl_mem,
        dst_buffer: cl_mem,
        src_origin: [number, number, number] | null | undefined,
        region: [number, number, number] | null | undefined,
        dst_offset: number,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueCopyBufferToImage(
        command_queue: cl_command_queue,
        src_buffer: cl_mem,
        dst_image: cl_mem,
        src_offset: number,
        dst_origin: [number, number, number] | null | undefined,
        region: [number, number, number] | null | undefined,
        event_wait_list?: cl_event[]
    ): cl_event;

    enqueueMapBuffer(
        command_queue: cl_command_queue,
        buffer: cl_mem,
        blocking_map: TRUE | FALSE | boolean,
        map_flags: MAP_READ | MAP_WRITE,
        offset: number,
        size: number,
        event_wait_list?: cl_event[] | null | undefined,
    ): ArrayBuffer & { event: cl_event };

    enqueueMapImage(
        command_queue: cl_command_queue,
        image: cl_mem,
        blocking_map: TRUE | FALSE | boolean,
        map_flags: MAP_READ | MAP_WRITE,
        origin: [number, number, number] | null | undefined,
        region: [number, number, number] | null | undefined,
        event_wait_list?: cl_event[] | null | undefined,
    ): ArrayBuffer & { event: cl_event, image_row_pitch: number, image_slice_pitch: number };

    enqueueUnmapMemObject(
        command_queue: cl_command_queue,
        memobj: cl_mem,
        mapped_ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueNDRangeKernel(
        command_queue: cl_command_queue,
        kernel: cl_kernel,
        work_dim: 1,
        global_work_offset: null, // As per specification for OpenCL 1.0 https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clEnqueueNDRangeKernel.html
        global_work_size: [number],
        local_work_size?: [number] | null | undefined,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;
    enqueueNDRangeKernel(
        command_queue: cl_command_queue,
        kernel: cl_kernel,
        work_dim: 2,
        global_work_offset: null, // As per specification for OpenCL 1.0 https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clEnqueueNDRangeKernel.html
        global_work_size: [number, number],
        local_work_size: [number, number] | null | undefined,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;
    enqueueNDRangeKernel(
        command_queue: cl_command_queue,
        kernel: cl_kernel,
        work_dim: 3,
        global_work_offset: null, // As per specification for OpenCL 1.0 https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clEnqueueNDRangeKernel.html
        global_work_size: [number, number, number],
        local_work_size: [number, number, number] | null | undefined,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    /* Deprecated OpenCL 1.1 APIs */
    createImage2D(
        context: cl_context,
        flags: Bitfield<MEM_READ_WRITE | MEM_WRITE_ONLY | MEM_READ_ONLY | MEM_USE_HOST_PTR | MEM_ALLOC_HOST_PTR | MEM_COPY_HOST_PTR>,
        image_format: cl_image_format,
        image_width: number,
        image_height: number,
        image_row_pitch: number,
        host_ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)
    ): cl_mem;

    unloadCompiler(): SUCCESS;

    // #ifdef USE_DEPRECATED_OPENCL_1_0_APIS
    /*
     *  WARNING:
     *     This API introduces mutable state into the OpenCL implementation. It has been REMOVED
     *  to better facilitate thread safety.  The 1.0 API is not thread safe. It is not tested by the
     *  OpenCL 1.1 conformance test, and consequently may not work or may not work dependably.
     *  It is likely to be non-performant. Use of this API is not advised. Use at your own risk.
     *
     *  Software developers previously relying on this API are instructed to set the command queue
     *  properties when creating the queue, instead.
     */
    // // Not implemented
    // setCommandQueueProperty(command_queue: cl_command_queue,
    //     properties: cl_command_queue_properties,
    //     enable: cl_bool,
    //     old_properties: Ptr<cl_command_queue_properties>): cl_int /* EXT_SUFFIX__v10_DEPRECATED */;
    // #endif /* USE_DEPRECATED_OPENCL_1_0_APIS */

    // // Not implemented
    // createImage3D(context: cl_context,
    //     flags: cl_mem_flags,
    //     image_format: Ptr<cl_image_format>,
    //     image_width: number,
    //     image_height: number,
    //     image_depth: number,
    //     image_row_pitch: number,
    //     image_slice_pitch: number,
    //     host_ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
    //     errcode_ret: Ptr<cl_int>): /* EXT_PREFIX__v11_DEPRECATED */ cl_mem /* EXT_SUFFIX__v11_DEPRECATED */;

    // // Not implemented
    // clGetExtensionFunctionAddress(func_name: string): (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) /* EXT_SUFFIX__v11_DEPRECATED */;
}
