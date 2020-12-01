/*
* @Author: mikael
* @Date:   2015-09-22 21:13:24
* @Last Modified by:   Mikael Bourges-Sevenier
* @Last Modified time: 2018-02-24 19:30:06
*/

'use strict';

import { OpenCL as cl, cl_platform_id, cl_kernel, cl_event } from '..';

let log = console.log;

saxpy();

function get_event_exec_time(event: cl_event) {
    // times are 64-bit values in nanoseconds. They are returned as [hi,lo] a 2-integer array
    // here we use the lo parts since this example is unlikely to go beyond 2^31 nanseconds per event.
    let start_time = cl.getEventProfilingInfo(event, cl.PROFILING_COMMAND_START);
    let end_time = cl.getEventProfilingInfo(event, cl.PROFILING_COMMAND_END);

    return (end_time[1] - start_time[1]) * 1e-6; // report in millisecond (from nanoseconds)
}

function createContext(platform: cl_platform_id) {
    let devices = cl.getDeviceIDs(platform, cl.DEVICE_TYPE_GPU);

    console.info("Found " + devices.length + " GPUs: ");

    for (let i = 0; i < devices.length; i++) {
        let name = cl.getDeviceInfo(devices[i], cl.DEVICE_NAME);

        console.info("  Devices " + i + ": " + name);
    }

    return cl.createContext([cl.CONTEXT_PLATFORM, platform], devices);
}

function saxpy() {
    let count = 512 * 1024;
    log("SAXPY with vector size: " + count + " elements");

    let alpha = 2.0;
    let A = new Float32Array(count);
    let B = new Float32Array(count);
    let C = new Float32Array(count);

    for (let i = 0; i < count; i++) {
        A[i] = i;
        B[i] = (count - i);
        C[i] = 0;
    }

    let [platform] = cl.getPlatformIDs();

    // create GPU context for this platform
    let context = cl.v20
        ? cl.createContextFromType([cl.CONTEXT_PLATFORM, cl.getPlatformIDs()[0]], cl.DEVICE_TYPE_GPU)
        : createContext(platform);

    let device = cl.getContextInfo(context, cl.CONTEXT_DEVICES)[0];
    log('Using device: ' + cl.getDeviceInfo(device, cl.DEVICE_NAME));

    // Create command queue
    let queue = cl.v20
        ? cl.createCommandQueueWithProperties(context, device, [cl.QUEUE_PROPERTIES, cl.QUEUE_PROFILING_ENABLE]) // OpenCL 2
        : cl.createCommandQueue(context, device, cl.QUEUE_PROFILING_ENABLE); // OpenCL 1.x

    let saxpy_kernel_source = [
        "__kernel                             ",
        "void saxpy_kernel(float alpha,       ",
        "                  __global float *A, ",
        "                  __global float *B, ",
        "                  __global float *C) ",
        "{                                    ",
        "    int idx = get_global_id(0);      ",
        "    C[idx] = alpha* A[idx] + B[idx]; ",
        "}                                    ",
    ].join("\n");

    // Create and program from source
    let program = cl.createProgramWithSource(context, saxpy_kernel_source);

    // Build program
    cl.buildProgram(program);

    let size = count * Float32Array.BYTES_PER_ELEMENT; // size in bytes

    // Create buffer for A and B and copy host contents
    let aBuffer = cl.createBuffer(context, cl.MEM_READ_ONLY, size);
    let bBuffer = cl.createBuffer(context, cl.MEM_READ_ONLY, size);

    // Create buffer for C to read results
    let cBuffer = cl.createBuffer(context, cl.MEM_WRITE_ONLY, size);

    // Create kernel object
    let kernel: cl_kernel;

    try {
        kernel = cl.createKernel(program, "saxpy_kernel");
    } catch (err) {
        console.log(cl.getProgramBuildInfo(program, device, cl.PROGRAM_BUILD_LOG));

        throw err;
    }

    // Do the work
    let write_events = [];
    write_events[0] = cl.enqueueWriteBuffer(queue, aBuffer, false, 0, size, A);
    write_events[1] = cl.enqueueWriteBuffer(queue, bBuffer, false, 0, size, B);

    // Execute (enqueue) kernel
    let global_work_size: [number] = [count]; // process entire list

    cl.setKernelArg(kernel, 0, "float", alpha);
    cl.setKernelArg(kernel, 1, "float*", aBuffer);
    cl.setKernelArg(kernel, 2, "float*", bBuffer);
    cl.setKernelArg(kernel, 3, "float*", cBuffer);
  
    let kernel_event = cl.enqueueNDRangeKernel(queue, kernel, 1, null, global_work_size, null, write_events);

    // get results and block while getting them
    let read_event = cl.enqueueReadBuffer(queue, cBuffer, false, 0, size, C, [kernel_event]);

    cl.waitForEvents([read_event]);
    // cl.finish(queue);
    log("C[last_value]=" + C[count - 1] + " should be " + (2 * (count - 1) + 1));

    // get all event statistics
    log("Time to transfer matrix A: " + get_event_exec_time(write_events[0]) + " ms");
    log("Time to transfer matrix B: " + get_event_exec_time(write_events[1]) + " ms");
    log("Time to execute SAXPY kernel: " + get_event_exec_time(kernel_event) + " ms");
    log("Time to read matrix C: " + get_event_exec_time(read_event) + " ms");
}

