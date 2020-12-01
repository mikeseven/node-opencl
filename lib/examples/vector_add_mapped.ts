/*
* @Author: mikael, lu4
* @Date:   2015-09-21 22:30:37
* @Last Modified by:   Mikael Bourges-Sevenier
* @Last Modified time: 2018-02-24 19:38:36
*/

'use strict';

import { OpenCL as cl, cl_kernel } from "..";

const log = console.log;
const BUFFER_SIZE = 10;

function VectorAdd() {
    const A = new Uint32Array(BUFFER_SIZE);
    const B = new Uint32Array(BUFFER_SIZE);
    const C = new Uint32Array(BUFFER_SIZE);

    for (let i = 0; i < BUFFER_SIZE; i++) {
        A[i] = i;
        B[i] = i * 2;
        C[i] = 10;
    }

    // create GPU context for this platform
    const platform = cl.getPlatformIDs()[0];
    const devices = cl.getDeviceIDs(platform, cl.DEVICE_TYPE_GPU);
    console.info("Found " + devices.length + " GPUs: ");

    let device = devices[0];
    for (let i = 0; i < devices.length; i++) {
        const name = cl.getDeviceInfo(devices[i], cl.DEVICE_NAME);
        console.info("  Devices " + i + ": " + name);
        if (name.indexOf("Intel") === -1) {
            // prefer discrete GPU 
            device = devices[i];
        }
    }

    const context = cl.createContext([cl.CONTEXT_PLATFORM, platform], [device]);
    // Alternatively:
    // const context = cl.createContextFromType([cl.CONTEXT_PLATFORM, platform], cl.DEVICE_TYPE_GPU);

    // const devices = cl.getContextInfo(context, cl.CONTEXT_DEVICES);
    // const device = devices[0];

    log('using device: ' + cl.getDeviceInfo(device, cl.DEVICE_VENDOR).trim() +
        ' ' + cl.getDeviceInfo(device, cl.DEVICE_NAME));

    const kernelSourceCode = [
        "__kernel void vadd(__global int *a, __global int *b, __global int *c, uint iNumElements) ",
        "{                                                                           ",
        "    size_t i =  get_global_id(0);                                           ",
        "    if(i > iNumElements) return;                                            ",
        "    c[i] = a[i] + b[i];                                                     ",
        "}                                                                           "
    ].join("\n");

    // Create and program from source
    const program = cl.createProgramWithSource(context, kernelSourceCode);

    // Build program
    cl.buildProgram(program);

    const size = BUFFER_SIZE * Uint32Array.BYTES_PER_ELEMENT; // size in bytes

    let kernel: cl_kernel;
    // Create kernel object
    try {
        kernel = cl.createKernel(program, "vadd");
    }
    catch (error) {
        console.log(cl.getProgramBuildInfo(program, device, cl.PROGRAM_BUILD_LOG));

        throw error;
    }

    // Create(Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)for A and copy host contents
    const aBuffer = cl.createBuffer(context, cl.MEM_READ_ONLY | cl.MEM_USE_HOST_PTR, size, A);

    // Create(Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)for B and copy host contents
    const bBuffer = cl.createBuffer(context, cl.MEM_READ_ONLY | cl.MEM_USE_HOST_PTR, size, B);

    // Create(Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)for that uses the host ptr C
    const cBuffer = cl.createBuffer(context, cl.MEM_WRITE_ONLY | cl.MEM_USE_HOST_PTR, size, C);

    // Set kernel args
    cl.setKernelArg(kernel, 0, "uint*", aBuffer);
    cl.setKernelArg(kernel, 1, "uint*", bBuffer);
    cl.setKernelArg(kernel, 2, "uint*", cBuffer);
    cl.setKernelArg(kernel, 3, "uint", BUFFER_SIZE);

    // Create command queue
    const queue = cl.v20
        ? cl.createCommandQueueWithProperties(context, device)  // OpenCL 2.x
        : cl.createCommandQueue(context, device); // OpenCL 1.x

    // Execute the OpenCL kernel on the list
    // const localWS = [5]; // process one list at a time
    // const global_work_size = [clu.roundUp(localWS, BUFFER_SIZE)]; // process entire list
    const global_work_size: [number] = [BUFFER_SIZE];

    log("Global work item size: " + global_work_size);

    // Execute (enqueue) kernel
    cl.enqueueNDRangeKernel(queue, kernel, 1, null, global_work_size);

    log("using enqueueMapBuffer");

    // Map cBuffer to host pointer. This enforces a sync with
    // the host backing space, remember we choose GPU device.
    const map = cl.enqueueMapBuffer(
        queue,
        cBuffer,      // cl buffer
        cl.TRUE,      // block
        cl.MAP_READ,  // flags
        0,            // offset
        size
    );

    let output = "after map C= ";

    for (let i = 0; i < BUFFER_SIZE; i++) {
        output += C[i] + ", ";
    }

    log(output);

    // we are now reading values as bytes, we need to cast it to the output type we want
    output = "output = [" + map.byteLength + " bytes] ";
    const map_view = new Uint8Array(map);
    for (let i = 0; i < map_view.length; i++) {
        output += map_view[i] + ", ";
    }
    log(output);

    cl.enqueueUnmapMemObject(queue, cBuffer, map);

    output = "after unmap C= ";
    for (let i = 0; i < BUFFER_SIZE; i++) {
        output += C[i] + ", ";
    }
    log(output);

    cl.finish(queue); // Finish all the operations

    printResults(A, B, C);

    // cleanup
    // cl.releaseAll();
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

    log(output);
}

VectorAdd();

console.log("\n== Main thread terminated ==");
