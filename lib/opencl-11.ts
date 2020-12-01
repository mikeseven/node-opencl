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
    cl_context,
    cl_event,
    cl_command_queue,
    cl_device_id,
    cl_kernel,
    cl_sampler,
    Bitfield
} from "./common";
import {
    EVENT_COMMAND_TYPE,
    COMMAND_NDRANGE_KERNEL,
    COMMAND_TASK,
    COMMAND_NATIVE_KERNEL,
    COMMAND_READ_BUFFER,
    COMMAND_WRITE_BUFFER,
    COMMAND_COPY_BUFFER,
    COMMAND_READ_IMAGE,
    COMMAND_WRITE_IMAGE,
    COMMAND_COPY_IMAGE,
    COMMAND_COPY_BUFFER_TO_IMAGE,
    COMMAND_COPY_IMAGE_TO_BUFFER,
    COMMAND_MAP_BUFFER,
    COMMAND_MAP_IMAGE,
    COMMAND_UNMAP_MEM_OBJECT,
    COMMAND_MARKER,
    COMMAND_ACQUIRE_GL_OBJECTS,
    COMMAND_RELEASE_GL_OBJECTS,
    TRUE,
    FALSE,
    MAP_WRITE,
    MAP_READ,
    SUCCESS,
    FILTER_LINEAR,
    FILTER_NEAREST,
    MEM_READ_WRITE,
    MEM_WRITE_ONLY,
    MEM_READ_ONLY,
    MEM_USE_HOST_PTR,
    MEM_ALLOC_HOST_PTR,
    MEM_COPY_HOST_PTR,
    COMPLETE,
    RUNNING,
    SUBMITTED,
    QUEUED,
    OpenCL_10_API
} from "./opencl-10";

// export type cl_buffer_create_type = cl_uint & { readonly cl_buffer_create_type: unique symbol };

// tslint:disable-next-line: class-name
export interface cl_buffer_region {
    origin: number;
    size: number;
} // _cl_buffer_region;

export type MISALIGNED_SUB_BUFFER_OFFSET = -13 & { readonly MISALIGNED_SUB_BUFFER_OFFSET: unique symbol };
export type EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST = -14 & { readonly EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: unique symbol };
export type INVALID_PROPERTY = -64 & { readonly INVALID_PROPERTY: unique symbol };
export type DEVICE_PREFERRED_VECTOR_WIDTH_HALF = 0x1034 & { readonly DEVICE_PREFERRED_VECTOR_WIDTH_HALF: unique symbol };
export type DEVICE_HOST_UNIFIED_MEMORY = 0x1035 & { readonly DEVICE_PREFERRED_VECTOR_WIDTH_HALF: unique symbol };
export type DEVICE_NATIVE_VECTOR_WIDTH_CHAR = 0x1036 & { readonly DEVICE_NATIVE_VECTOR_WIDTH_CHAR: unique symbol };
export type DEVICE_NATIVE_VECTOR_WIDTH_SHORT = 0x1037 & { readonly DEVICE_NATIVE_VECTOR_WIDTH_SHORT: unique symbol };
export type DEVICE_NATIVE_VECTOR_WIDTH_INT = 0x1038 & { readonly DEVICE_NATIVE_VECTOR_WIDTH_INT: unique symbol };
export type DEVICE_NATIVE_VECTOR_WIDTH_LONG = 0x1039 & { readonly DEVICE_NATIVE_VECTOR_WIDTH_LONG: unique symbol };
export type DEVICE_NATIVE_VECTOR_WIDTH_FLOAT = 0x103A & { readonly DEVICE_NATIVE_VECTOR_WIDTH_FLOAT: unique symbol };
export type DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE = 0x103B & { readonly DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE: unique symbol };
export type DEVICE_NATIVE_VECTOR_WIDTH_HALF = 0x103C & { readonly DEVICE_NATIVE_VECTOR_WIDTH_HALF: unique symbol };
export type DEVICE_OPENCL_C_VERSION = 0x103D & { readonly DEVICE_OPENCL_C_VERSION: unique symbol };
export type FP_SOFT_FLOAT = 64 & { readonly FP_SOFT_FLOAT: unique symbol };
export type CONTEXT_NUM_DEVICES = 0x1083 & { readonly CONTEXT_NUM_DEVICES: unique symbol };
export type Rx = 0x10BA & { readonly Rx: unique symbol };
export type RGx = 0x10BB & { readonly RGx: unique symbol };
export type RGBx = 0x10BC & { readonly RGBx: unique symbol };
export type MEM_ASSOCIATED_MEMOBJECT = 0x1107 & { readonly MEM_ASSOCIATED_MEMOBJECT: unique symbol };
export type MEM_OFFSET = 0x1108 & { readonly MEM_OFFSET: unique symbol };
export type ADDRESS_MIRRORED_REPEAT = 0x1134 & { readonly ADDRESS_MIRRORED_REPEAT: unique symbol };
export type EVENT_CONTEXT = 0x11D4 & { readonly EVENT_CONTEXT: unique symbol };
export type COMMAND_READ_BUFFER_RECT = 0x1201 & { readonly COMMAND_READ_BUFFER_RECT: unique symbol };
export type COMMAND_WRITE_BUFFER_RECT = 0x1202 & { readonly COMMAND_WRITE_BUFFER_RECT: unique symbol };
export type COMMAND_COPY_BUFFER_RECT = 0x1203 & { readonly COMMAND_COPY_BUFFER_RECT: unique symbol };
export type COMMAND_USER = 0x1204 & { readonly COMMAND_USER: unique symbol };
export type BUFFER_CREATE_TYPE_REGION = 0x1220 & { readonly BUFFER_CREATE_TYPE_REGION: unique symbol };

// List of members deliberately removed from from the specification

// tslint:disable-next-line: no-empty-interface class-name
export interface OpenCL_11_UNINTENDED_MEMBERS {
    // setMemObjectDestructorCallback<TUserData>(memobj: cl_mem, pfn_notify: (memobj: cl_mem, user_data: TUserData) => void, user_data: TUserData): cl_int;
}

export type OpenCL_11 = OpenCL_11_API & OpenCL_11_MEMBERS_THAT_ARE_NOT_INCLUDDED_IN_FUTURE_VERSIONS;
export type OpenCL_11_API = OpenCL_11_MEMBERS & OpenCL_11_IMPROVED_MEMBERS;

// tslint:disable-next-line: class-name
export interface OpenCL_11_MEMBERS_THAT_ARE_NOT_INCLUDDED_IN_FUTURE_VERSIONS {
    version: '1.1',
    v10: true,
    v11: true,
    v12: false,
    v20: false,
    v21: false,
    v22: false
}

// List of new members that are known not to be removed in future versions of OpenCL
// tslint:disable-next-line: class-name
export interface OpenCL_11_IMPROVED_MEMBERS {
    createSampler(
        context: cl_context,
        normalized_coords: TRUE | FALSE | boolean,
        addressing_mode: ADDRESS_MIRRORED_REPEAT,
        filter_mode: FILTER_NEAREST | FILTER_LINEAR
    ): cl_sampler;

    getEventInfo(
        event: cl_event,
        param_name: EVENT_CONTEXT
    ): cl_context;
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
        COMMAND_RELEASE_GL_OBJECTS |
        COMMAND_READ_BUFFER_RECT |
        COMMAND_WRITE_BUFFER_RECT |
        COMMAND_COPY_BUFFER_RECT |
        COMMAND_USER;

    getDeviceInfo(device: cl_device_id, param_name: DEVICE_OPENCL_C_VERSION): string;
    getDeviceInfo(device: cl_device_id, param_name: DEVICE_HOST_UNIFIED_MEMORY): boolean;
    getDeviceInfo(device: cl_device_id,
        param_name: DEVICE_NATIVE_VECTOR_WIDTH_CHAR |
            DEVICE_NATIVE_VECTOR_WIDTH_SHORT |
            DEVICE_NATIVE_VECTOR_WIDTH_INT |
            DEVICE_NATIVE_VECTOR_WIDTH_LONG |
            DEVICE_NATIVE_VECTOR_WIDTH_FLOAT |
            DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE |
            DEVICE_NATIVE_VECTOR_WIDTH_HALF |
            DEVICE_PREFERRED_VECTOR_WIDTH_HALF
    ): number;

    getContextInfo(context: cl_context, param_name: CONTEXT_NUM_DEVICES): number;

    getMemObjectInfo(
        memobj: cl_mem,
        param_name: MEM_OFFSET
    ): number;

    getMemObjectInfo(
        memobj: cl_mem,
        param_name: MEM_ASSOCIATED_MEMOBJECT
    ): cl_mem;

    enqueueMapBuffer(
        command_queue: cl_command_queue,
        buffer: cl_mem,
        blocking_map: TRUE | FALSE | boolean,
        map_flags: MAP_READ | MAP_WRITE,
        offset: number,
        size: number,
        event_wait_list: cl_event[] | null | undefined,
        event: cl_event | null | undefined
    ): ArrayBuffer & { event: cl_event };

    enqueueMapImage(
        command_queue: cl_command_queue,
        image: cl_mem,
        blocking_map: TRUE | FALSE | boolean,
        map_flags: MAP_READ | MAP_WRITE,
        origin: [number, number, number] | null | undefined,
        region: [number, number, number] | null | undefined,
        event_wait_list: cl_event[] | null | undefined,
        event: cl_event | null | undefined
    ): ArrayBuffer & { event: cl_event, image_row_pitch: number, image_slice_pitch: number };
    enqueueMapImage(
        command_queue: cl_command_queue,
        image: cl_mem,
        blocking_map: TRUE | FALSE | boolean,
        map_flags: MAP_READ | MAP_WRITE,
        origin: [number, number, number] | null | undefined,
        region: [number, number, number] | null | undefined,
        event_wait_list?: cl_event[] | null | undefined
    ): ArrayBuffer & { image_row_pitch: number, image_slice_pitch: number };

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
        global_work_offset: [number],
        global_work_size: [number],
        local_work_size: [number],
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;
    enqueueNDRangeKernel(
        command_queue: cl_command_queue,
        kernel: cl_kernel,
        work_dim: 2,
        global_work_offset: [number, number],
        global_work_size: [number, number],
        local_work_size: [number, number],
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;
    enqueueNDRangeKernel(
        command_queue: cl_command_queue,
        kernel: cl_kernel,
        work_dim: 3,
        global_work_offset: [number, number, number],
        global_work_size: [number, number, number],
        local_work_size: [number, number, number],
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;
}

// tslint:disable-next-line: class-name
export interface OpenCL_11_MEMBERS extends OpenCL_10_API {
    MISALIGNED_SUB_BUFFER_OFFSET: MISALIGNED_SUB_BUFFER_OFFSET;
    EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
    INVALID_PROPERTY: INVALID_PROPERTY;
    DEVICE_PREFERRED_VECTOR_WIDTH_HALF: DEVICE_PREFERRED_VECTOR_WIDTH_HALF;
    // deprecated in 2.0, not yet removed from current implementation
    DEVICE_HOST_UNIFIED_MEMORY: DEVICE_HOST_UNIFIED_MEMORY;
    DEVICE_NATIVE_VECTOR_WIDTH_CHAR: DEVICE_NATIVE_VECTOR_WIDTH_CHAR;
    DEVICE_NATIVE_VECTOR_WIDTH_SHORT: DEVICE_NATIVE_VECTOR_WIDTH_SHORT;
    DEVICE_NATIVE_VECTOR_WIDTH_INT: DEVICE_NATIVE_VECTOR_WIDTH_INT;
    DEVICE_NATIVE_VECTOR_WIDTH_LONG: DEVICE_NATIVE_VECTOR_WIDTH_LONG;
    DEVICE_NATIVE_VECTOR_WIDTH_FLOAT: DEVICE_NATIVE_VECTOR_WIDTH_FLOAT;
    DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE: DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE;
    DEVICE_NATIVE_VECTOR_WIDTH_HALF: DEVICE_NATIVE_VECTOR_WIDTH_HALF;
    DEVICE_OPENCL_C_VERSION: DEVICE_OPENCL_C_VERSION;
    FP_SOFT_FLOAT: FP_SOFT_FLOAT;
    CONTEXT_NUM_DEVICES: CONTEXT_NUM_DEVICES;
    Rx: Rx;
    RGx: RGx;
    RGBx: RGBx;
    MEM_ASSOCIATED_MEMOBJECT: MEM_ASSOCIATED_MEMOBJECT;
    MEM_OFFSET: MEM_OFFSET;
    ADDRESS_MIRRORED_REPEAT: ADDRESS_MIRRORED_REPEAT;
    EVENT_CONTEXT: EVENT_CONTEXT;
    COMMAND_READ_BUFFER_RECT: COMMAND_READ_BUFFER_RECT;
    COMMAND_WRITE_BUFFER_RECT: COMMAND_WRITE_BUFFER_RECT;
    COMMAND_COPY_BUFFER_RECT: COMMAND_COPY_BUFFER_RECT;
    COMMAND_USER: COMMAND_USER;
    BUFFER_CREATE_TYPE_REGION: BUFFER_CREATE_TYPE_REGION;

    createSubBuffer(
        buffer: cl_mem,
        flags: Bitfield<MEM_READ_WRITE | MEM_WRITE_ONLY | MEM_READ_ONLY | MEM_USE_HOST_PTR | MEM_ALLOC_HOST_PTR | MEM_COPY_HOST_PTR>,
        buffer_create_type: BUFFER_CREATE_TYPE_REGION,
        buffer_create_info: cl_buffer_region
    ): cl_mem;

    createUserEvent(
        context: cl_context
    ): cl_event;

    setUserEventStatus(
        event: cl_event,
        execution_status: COMPLETE | -1
    ): SUCCESS;

    setEventCallback<TUserData>(
        event: cl_event,
        command_exec_callback_type: COMPLETE | RUNNING | SUBMITTED | QUEUED,
        pfn_notify: (event: cl_event, event_command_status: QUEUED | SUBMITTED | RUNNING | COMPLETE, user_data: TUserData) => void,
        user_data: TUserData
    ): SUCCESS;
    setEventCallback(
        event: cl_event,
        command_exec_callback_type: COMPLETE | RUNNING | SUBMITTED | QUEUED,
        pfn_notify: (event: cl_event, event_command_status: QUEUED | SUBMITTED | RUNNING | COMPLETE, user_data?: null | undefined) => void
    ): SUCCESS;

    enqueueReadBufferRect(
        command_queue: cl_command_queue,
        buffer: cl_mem,
        blocking_read: TRUE | FALSE | boolean,
        buffer_offset: [number, number, number] | null | undefined,
        host_offset: [number, number, number] | null | undefined,
        region: [number, number, number] | null | undefined,
        buffer_row_pitch: number,
        buffer_slice_pitch: number,
        host_row_pitch: number,
        host_slice_pitch: number,
        ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueReadBufferRect(
        command_queue: cl_command_queue,
        buffer: cl_mem,
        blocking_read: TRUE | FALSE | boolean,
        buffer_offset: [number, number] | null | undefined,
        host_offset: [number, number] | null | undefined,
        region: [number, number] | null | undefined,
        buffer_row_pitch: number,
        buffer_slice_pitch: number,
        host_row_pitch: number,
        host_slice_pitch: number,
        ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;
    enqueueReadBufferRect(
        command_queue: cl_command_queue,
        buffer: cl_mem,
        blocking_read: TRUE | FALSE | boolean,
        buffer_offset: [number] | null | undefined,
        host_offset: [number] | null | undefined,
        region: [number] | null | undefined,
        buffer_row_pitch: number,
        buffer_slice_pitch: number,
        host_row_pitch: number,
        host_slice_pitch: number,
        ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueWriteBufferRect(
        command_queue: cl_command_queue,
        buffer: cl_mem,
        blocking_read: TRUE | FALSE | boolean,
        buffer_offset: [number, number, number] | null | undefined,
        host_offset: [number, number, number] | null | undefined,
        region: [number, number, number] | null | undefined,
        buffer_row_pitch: number,
        buffer_slice_pitch: number,
        host_row_pitch: number,
        host_slice_pitch: number,
        ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;
    enqueueWriteBufferRect(
        command_queue: cl_command_queue,
        buffer: cl_mem,
        blocking_read: TRUE | FALSE | boolean,
        buffer_offset: [number, number] | null | undefined,
        host_offset: [number, number] | null | undefined,
        region: [number, number] | null | undefined,
        buffer_row_pitch: number,
        buffer_slice_pitch: number,
        host_row_pitch: number,
        host_slice_pitch: number,
        ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;
    enqueueWriteBufferRect(
        command_queue: cl_command_queue,
        buffer: cl_mem,
        blocking_read: TRUE | FALSE | boolean,
        buffer_offset: [number] | null | undefined,
        host_offset: [number] | null | undefined,
        region: [number] | null | undefined,
        buffer_row_pitch: number,
        buffer_slice_pitch: number,
        host_row_pitch: number,
        host_slice_pitch: number,
        ptr: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer),
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueCopyBufferRect(
        command_queue: cl_command_queue,
        src_buffer: cl_mem,
        dst_buffer: cl_mem,
        src_origin: [number, number, number] | null | undefined,
        dst_origin: [number, number, number] | null | undefined,
        region: [number, number, number] | null | undefined,
        src_row_pitch: number,
        src_slice_pitch: number,
        dst_row_pitch: number,
        dst_slice_pitch: number,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;
    enqueueCopyBufferRect(
        command_queue: cl_command_queue,
        src_buffer: cl_mem,
        dst_buffer: cl_mem,
        src_origin: [number, number] | null | undefined,
        dst_origin: [number, number] | null | undefined,
        region: [number, number] | null | undefined,
        src_row_pitch: number,
        src_slice_pitch: number,
        dst_row_pitch: number,
        dst_slice_pitch: number,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;
    enqueueCopyBufferRect(
        command_queue: cl_command_queue,
        src_buffer: cl_mem,
        dst_buffer: cl_mem,
        src_origin: [number] | null | undefined,
        dst_origin: [number] | null | undefined,
        region: [number] | null | undefined,
        src_row_pitch: number,
        src_slice_pitch: number,
        dst_row_pitch: number,
        dst_slice_pitch: number,
        event_wait_list?: cl_event[] | null | undefined
    ): cl_event;

    enqueueMarker(
        command_queue: cl_command_queue
    ): cl_event;

    enqueueWaitForEvents(
        command_queue: cl_command_queue,
        event_list: cl_event[]
    ): SUCCESS;

    enqueueBarrier(
        command_queue: cl_command_queue
    ): SUCCESS;
}
