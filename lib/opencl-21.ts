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
    Ptr,
    cl_uint,
    cl_context,
    cl_int,
    cl_command_queue,
    cl_device_id,
    cl_ulong,
    cl_program,
    cl_kernel,
    cl_event,
    cl_bitfield
} from "./common";

import { OpenCL_20_API } from "./opencl-20";

export type cl_kernel_sub_group_info = cl_uint;
export type cl_mem_migration_flags = cl_bitfield & { readonly cl_mem_migration_flags: unique symbol };

export type PLATFORM_HOST_TIMER_RESOLUTION = 0x0905 & { readonly PLATFORM_HOST_TIMER_RESOLUTION: unique symbol };
export type DEVICE_IL_VERSION = 0x105B & { readonly DEVICE_IL_VERSION: unique symbol };
export type DEVICE_MAX_NUM_SUB_GROUPS = 0x105C & { readonly DEVICE_MAX_NUM_SUB_GROUPS: unique symbol };
export type DEVICE_SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS = 0x105D & { readonly DEVICE_SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS: unique symbol };
export type QUEUE_DEVICE_DEFAULT = 0x1095 & { readonly QUEUE_DEVICE_DEFAULT: unique symbol };
export type UNORM_INT_101010_2 = 0x10E0 & { readonly UNORM_INT_101010_2: unique symbol };
export type PROGRAM_IL = 0x1169 & { readonly PROGRAM_IL: unique symbol };
export type KERNEL_MAX_NUM_SUB_GROUPS = 0x11B9 & { readonly KERNEL_MAX_NUM_SUB_GROUPS: unique symbol };
export type KERNEL_COMPILE_NUM_SUB_GROUPS = 0x11BA & { readonly KERNEL_COMPILE_NUM_SUB_GROUPS: unique symbol };

/* cl_kernel_sub_group_info */
export type KERNEL_MAX_SUB_GROUP_SIZE_FOR_NDRANGE = 0x2033 & { readonly KERNEL_MAX_SUB_GROUP_SIZE_FOR_NDRANGE: unique symbol };
export type KERNEL_SUB_GROUP_COUNT_FOR_NDRANGE = 0x2034 & { readonly KERNEL_SUB_GROUP_COUNT_FOR_NDRANGE: unique symbol };
export type KERNEL_LOCAL_SIZE_FOR_SUB_GROUP_COUNT = 0x11B8 & { readonly KERNEL_LOCAL_SIZE_FOR_SUB_GROUP_COUNT: unique symbol };

export type OpenCL_21 = OpenCL_21_API & OpenCL_21_MEMBERS_THAT_ARE_NOT_INCLUDDED_IN_FUTURE_VERSIONS;

export type OpenCL_21_API = OpenCL_21_MEMBERS & OpenCL_21_IMPROVED_MEMBERS;

export interface OpenCL_21_MEMBERS_THAT_ARE_NOT_INCLUDDED_IN_FUTURE_VERSIONS {
    version: '2.1',
    v10: true,
    v11: true,
    v12: true,
    v20: true,
    v21: true,
    v22: false
}

export interface OpenCL_21_IMPROVED_MEMBERS { }

export interface OpenCL_21_MEMBERS extends OpenCL_20_API {
    PLATFORM_HOST_TIMER_RESOLUTION: PLATFORM_HOST_TIMER_RESOLUTION;
    DEVICE_IL_VERSION: DEVICE_IL_VERSION;
    DEVICE_MAX_NUM_SUB_GROUPS: DEVICE_MAX_NUM_SUB_GROUPS;
    DEVICE_SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS: DEVICE_SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS;
    QUEUE_DEVICE_DEFAULT: QUEUE_DEVICE_DEFAULT;
    UNORM_INT_101010_2: UNORM_INT_101010_2;
    PROGRAM_IL: PROGRAM_IL;
    KERNEL_MAX_NUM_SUB_GROUPS: KERNEL_MAX_NUM_SUB_GROUPS;
    KERNEL_COMPILE_NUM_SUB_GROUPS: KERNEL_COMPILE_NUM_SUB_GROUPS;
    KERNEL_MAX_SUB_GROUP_SIZE_FOR_NDRANGE: KERNEL_MAX_SUB_GROUP_SIZE_FOR_NDRANGE;
    KERNEL_SUB_GROUP_COUNT_FOR_NDRANGE: KERNEL_SUB_GROUP_COUNT_FOR_NDRANGE;
    KERNEL_LOCAL_SIZE_FOR_SUB_GROUP_COUNT: KERNEL_LOCAL_SIZE_FOR_SUB_GROUP_COUNT;

    setDefaultDeviceCommandQueue(context: cl_context,
        device: cl_device_id,
        command_queue: cl_command_queue): cl_int /* API_SUFFIX__v21 */;

    getDeviceAndHostTimer(device: cl_device_id,
        device_timestamp: Ptr<cl_ulong>,
        host_timestamp: Ptr<cl_ulong>): cl_int /* API_SUFFIX__v21 */;

    getHostTimer(device: cl_device_id,
        host_timestamp: Ptr<cl_ulong>): cl_int /* API_SUFFIX__v21 */;

    createProgramWithIL(context: cl_context,
        il: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        length: number,
        errcode_ret: Ptr<cl_int>): cl_program /* API_SUFFIX__v21 */;

    cloneKernel(source_kernel: cl_kernel,
        errcode_ret: Ptr<cl_int>): cl_kernel /* API_SUFFIX__v21 */;

    getKernelSubGroupInfo(kernel: cl_kernel,
        device: cl_device_id,
        param_name: cl_kernel_sub_group_info,
        input_value_size: number,
        input_value: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        param_value_size: number,
        param_value: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        param_value_size_ret: Ptr<number>): cl_int /* API_SUFFIX__v21 */;

    enqueueSVMMigrateMem(
        command_queue: cl_command_queue,
        num_svm_pointers: cl_uint,
        svm_pointers: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)[],
        sizes: Ptr<number>,
        flags: cl_mem_migration_flags,
        num_events_in_wait_list: cl_uint,
        event_wait_list: Ptr<cl_event>,
        event: Ptr<cl_event>): cl_int /* API_SUFFIX__v21 */;
}