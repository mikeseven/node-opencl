/*
* @Author: mikael
* @Date:   2015-09-21 18:06:21
* @Last Modified by:   mikael
* @Last Modified time: 2015-12-08 21:00:39
*/

// 'use strict';

import { OpenCL as cl } from "..";

let BUFFER_SIZE = 10;

function VectorAdd() {
    let A = new Uint32Array(BUFFER_SIZE);
    let B = new Uint32Array(BUFFER_SIZE);

    for (let i = 0; i < BUFFER_SIZE; i++) {
        A[i] = i;
        B[i] = i * 2;
    }

    let platforms = cl.getPlatformIDs();
    for (let i = 0; i < platforms.length; i++) {
        console.info("Platform " + i + ": " + cl.getPlatformInfo(platforms[i], cl.PLATFORM_NAME));
    }
    let platform = platforms[0];

    let devices = cl.getDeviceIDs(platform, cl.DEVICE_TYPE_ALL);
    for (let i = 0; i < devices.length; i++) {
        console.info("  Devices " + i + ": " + cl.getDeviceInfo(devices[i], cl.DEVICE_NAME));
    }

    console.info("creating context");

    // var context = cl.createContextFromType(
    //   [cl.CONTEXT_PLATFORM, platform],
    //   cl.DEVICE_TYPE_GPU);
    let context = cl.createContext([
        cl.CONTEXT_PLATFORM, platform],
        devices);

    console.info("created context");

    let kernelSourceCode = [
        "__kernel void vadd(__global int *a, __global int *b, __global int *c, uint iNumElements) ",
        "{                                                                           ",
        "    size_t i =  get_global_id(0);                                           ",
        "    if(i >= iNumElements) return;                                           ",
        "    c[i] = a[i] + b[i];                                                     ",
        "}                                                                           "
    ].join("\n");

    // Create and program from source
    let program = cl.createProgramWithSource(context, kernelSourceCode);

    // Build program
    cl.buildProgram(program);

    let size = BUFFER_SIZE * Uint32Array.BYTES_PER_ELEMENT; // size in bytes

    // Create(Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)for A and B and copy host contents
    let aBuffer = cl.createBuffer(context, cl.MEM_READ_ONLY, size);
    let bBuffer = cl.createBuffer(context, cl.MEM_READ_ONLY, size);

    // Create(Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)for C to read results
    let cBuffer = cl.createBuffer(context, cl.MEM_WRITE_ONLY, size);

    let device = cl.getContextInfo(context, cl.CONTEXT_DEVICES)[0];

    // Create kernel object
    let kernel;
    try {
        kernel = cl.createKernel(program, "vadd");
    } catch (error) {
        console.error(cl.getProgramBuildInfo(program, device, cl.PROGRAM_BUILD_LOG));
        throw error;
    }

    // Set kernel args
    cl.setKernelArg(kernel, 0, "uint*", aBuffer);
    cl.setKernelArg(kernel, 1, "uint*", bBuffer);
    cl.setKernelArg(kernel, 2, "uint*", cBuffer);
    cl.setKernelArg(kernel, 3, "uint", BUFFER_SIZE);

    // Create command queue
    let queue = cl.v20
        ? cl.createCommandQueueWithProperties(context, device, []) // OpenCL 2;
        : cl.createCommandQueue(context, device, null); // OpenCL 1.x

    // Do the work
    cl.enqueueWriteBuffer(queue, aBuffer, true, 0, A.length * Uint32Array.BYTES_PER_ELEMENT, A);
    cl.enqueueWriteBuffer(queue, bBuffer, true, 0, B.length * Uint32Array.BYTES_PER_ELEMENT, B);

    // Execute (enqueue) kernel
    console.info("using enqueueNDRangeKernel");
    cl.enqueueNDRangeKernel(queue, kernel, 1,
        null,
        [BUFFER_SIZE],
        null);

    // get results and block while getting them
    let C = new Uint32Array(BUFFER_SIZE);
    cl.enqueueReadBuffer(queue, cBuffer, true, 0, C.length * Uint32Array.BYTES_PER_ELEMENT, C);

    // print results
    printResults(A, B, C);

    // cleanup
    // test release each CL object
    cl.releaseCommandQueue(queue);
    cl.releaseKernel(kernel);
    cl.releaseProgram(program);
    cl.releaseMemObject(aBuffer);
    cl.releaseMemObject(bBuffer);
    cl.releaseMemObject(cBuffer);
    cl.releaseContext(context);

    // test release all CL objects
    // cl.releaseAll();

    // if no manual cleanup specified, cl.releaseAll() is called at exit of program
}

function printResults(A: Uint32Array, B: Uint32Array, C: Uint32Array) {
    // Print input vectors and result vector
    let output = "\nA = ";
    for (let i = 0; i < BUFFER_SIZE; i++) {
        output += A[i] + ", ";
    }
    output += "\nB = ";
    for (let i = 0; i < BUFFER_SIZE; i++) {
        output += B[i] + ", ";
    }
    output += "\nC = ";
    for (let i = 0; i < BUFFER_SIZE; i++) {
        output += C[i] + ", ";
    }

    console.info(output);
}

VectorAdd();

// Main thread will always finish before CL callbacks are finished.
// Calling process.exit() in the main thread would skip CL callbacks from executing
console.info("\n== Main thread terminated ==");
