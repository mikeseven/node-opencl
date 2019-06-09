import * as fs from 'fs';
import { OpenCL as cl } from "..";

function Square() {
    console.log("Using Buffer");

    const [platform] = cl.getPlatformIDs();
    const ctx = cl.v20
        ? cl.createContextFromType([cl.CONTEXT_PLATFORM, platform], cl.DEVICE_TYPE_ALL)
        : cl.createContext([cl.CONTEXT_PLATFORM, platform], cl.getDeviceIDs(platform, cl.DEVICE_TYPE_ALL));

    // Alternatively:
    // const ctx = ;

    const elements = 100;
    const elementSize = Uint32Array.BYTES_PER_ELEMENT;

    const inputs = Buffer.alloc(elements * elementSize);
    const outputs = Buffer.alloc(elements * elementSize);

    outputs.fill(0);

    // Note: using little endian for Intel-based machines, GPU follows same convention
    // as CPU typically but it should be detected with clGetDeviceInfo(CL_DEVICE_ENDIAN_LITTLE)

    for (let i = 0; i < elements; i++) {
        inputs.writeUInt32LE(i, i * elementSize); // inputs[offset]=i with offset=i*4 since each uint32 value takes 4 bytes
    }

    const source = fs.readFileSync(__dirname + "/square.cl").toString();

    const prog = cl.createProgramWithSource(ctx, source);

    cl.buildProgram(prog);

    const kern = cl.createKernel(prog, "square");

    const inputsMem = cl.createBuffer(ctx, cl.MEM_READ_ONLY | cl.MEM_COPY_HOST_PTR, elements * elementSize, inputs);
    const outputsMem = cl.createBuffer(ctx, cl.MEM_WRITE_ONLY | cl.MEM_COPY_HOST_PTR, elements * elementSize, outputs);

    cl.setKernelArg(kern, 0, "uint*", inputsMem);
    cl.setKernelArg(kern, 1, "uint*", outputsMem);
    cl.setKernelArg(kern, 2, "uint", elements);

    const device = cl.getContextInfo(ctx, cl.CONTEXT_DEVICES)[0];
    const commandQueue = cl.v20
        ? cl.createCommandQueueWithProperties(ctx, device, [])
        : cl.createCommandQueue(ctx, device, null); // OpenCL 1.x

    cl.enqueueNDRangeKernel(commandQueue, kern, 1, null, [elements], null);
    cl.enqueueReadBuffer(commandQueue, outputsMem, true, 0, elements * elementSize, outputs); // should contains i^2 for i=0,...,10000-1

    cl.finish(commandQueue);

    console.log("#elems in outputs: " + outputs.length);

    const last_value = outputs.readUInt32LE(elementSize * (elements - 1));

    console.log("Last value is : " + last_value + " should be " + ((elements - 1) * (elements - 1)));
}

function Square_TypedArray() {
    console.log("Using TypedArrays");

    const [platform] = cl.getPlatformIDs();
    const ctx = cl.v20
        ? cl.createContextFromType([cl.CONTEXT_PLATFORM, platform], cl.DEVICE_TYPE_ALL)
        : cl.createContext([cl.CONTEXT_PLATFORM, platform], cl.getDeviceIDs(platform, cl.DEVICE_TYPE_ALL));

    const elements = 100;
    const BYTES_PER_ELEMENT = Uint32Array.BYTES_PER_ELEMENT;

    const inputs = new Uint32Array(elements);
    const outputs = new Uint32Array(elements);

    outputs.fill(0);

    for (let i = 0; i < elements; i++) {
        inputs[i] = i;
    }

    const source = fs.readFileSync(__dirname + "/square.cl").toString();

    const prog = cl.createProgramWithSource(ctx, source);

    cl.buildProgram(prog);

    const kern = cl.createKernel(prog, "square");

    const inputsMem = cl.createBuffer(ctx, cl.MEM_READ_ONLY | cl.MEM_COPY_HOST_PTR, elements * BYTES_PER_ELEMENT, inputs);
    const outputsMem = cl.createBuffer(ctx, cl.MEM_WRITE_ONLY | cl.MEM_COPY_HOST_PTR, elements * BYTES_PER_ELEMENT, outputs);

    cl.setKernelArg(kern, 0, "uint*", inputsMem);
    cl.setKernelArg(kern, 1, "uint*", outputsMem);
    cl.setKernelArg(kern, 2, "uint", elements);

    const device = cl.getContextInfo(ctx, cl.CONTEXT_DEVICES)[0];
    const commandQueue = cl.v20
        ? cl.createCommandQueueWithProperties(ctx, device, [])
        : cl.createCommandQueue(ctx, device, null);

    cl.enqueueNDRangeKernel(commandQueue, kern, 1, null, [elements], null);
    cl.enqueueReadBuffer(commandQueue, outputsMem, true, 0, elements * BYTES_PER_ELEMENT, outputs); // should contains i^2 for i=0,...,10000-1

    cl.finish(commandQueue);

    console.log("#elems in outputs: " + outputs.length);
    const last_value = outputs[elements - 1];
    console.log("Last value is : " + last_value + " should be " + ((elements - 1) * (elements - 1)));
}

Square();

console.log();

Square_TypedArray();

console.log("\n== Program terminated ==");
