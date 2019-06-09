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

import { cl_program, cl_int, cl_uint } from "./common";
import { OpenCL_21_API } from "./opencl-21";

export type INVALID_SPEC_ID = -71 & { readonly INVALID_SPEC_ID: unique symbol };
export type MAX_SIZE_RESTRICTION_EXCEEDED = -72 & { readonly MAX_SIZE_RESTRICTION_EXCEEDED: unique symbol };
export type PROGRAM_SCOPE_GLOBAL_CTORS_PRESENT = 0x116A & { readonly PROGRAM_SCOPE_GLOBAL_CTORS_PRESENT: unique symbol };
export type PROGRAM_SCOPE_GLOBAL_DTORS_PRESENT = 0x116B & { readonly PROGRAM_SCOPE_GLOBAL_DTORS_PRESENT: unique symbol };

export type OpenCL_22 = OpenCL_22_API & OpenCL_22_MEMBERS_THAT_ARE_NOT_INCLUDDED_IN_FUTURE_VERSIONS;
export type OpenCL_22_API = OpenCL_22_MEMBERS;

export interface OpenCL_22_MEMBERS_THAT_ARE_NOT_INCLUDDED_IN_FUTURE_VERSIONS {
    version: '2.2',
    v10: true,
    v11: true,
    v12: true,
    v20: true,
    v21: true,
    v22: true
}

export interface OpenCL_22_MEMBERS extends OpenCL_21_API {
    INVALID_SPEC_ID: INVALID_SPEC_ID;
    MAX_SIZE_RESTRICTION_EXCEEDED: MAX_SIZE_RESTRICTION_EXCEEDED;
    PROGRAM_SCOPE_GLOBAL_CTORS_PRESENT: PROGRAM_SCOPE_GLOBAL_CTORS_PRESENT;
    PROGRAM_SCOPE_GLOBAL_DTORS_PRESENT: PROGRAM_SCOPE_GLOBAL_DTORS_PRESENT;

    setProgramReleaseCallback(program: cl_program,
        pfn_notify: (program: cl_program, user_data: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)) => void,
        user_data: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)
    ): cl_int /* API_SUFFIX__v22 */;

    setProgramSpecializationConstant(program: cl_program,
        spec_id: cl_uint,
        spec_size: number,
        spec_value: (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)
    ): cl_int /* API_SUFFIX__v22 */;

}