'use strict';

var cl = require("../lib/opencl");
var fs = require("fs");

function Square() {
    console.log("Using Buffer");

    var ctx;
    if (cl.createContextFromType !== undefined) {
        ctx = cl.createContextFromType(
            [cl.CONTEXT_PLATFORM, cl.getPlatformIDs()[0]], cl.DEVICE_TYPE_ALL, null, null);
    }
    else {
        var platform = cl.getPlatformIDs()[0];
        ctx = cl.createContext(
            [cl.CONTEXT_PLATFORM, platform],
            [cl.getDeviceIDs(platform, cl.DEVICE_TYPE_ALL)[0]]);
    }

    var NVALUES = 100;
    var BYTES_PER_ELEMENT = Uint32Array.BYTES_PER_ELEMENT;

    var inputs = Buffer.alloc(NVALUES * BYTES_PER_ELEMENT); // *4 because uint is 4 bytes.
    var outputs = Buffer.alloc(NVALUES * BYTES_PER_ELEMENT);
    outputs.fill(0);

    // Note: using little endian for Intel-based machines, GPU follows same convention
    // as CPU typically but it should be detected with clGetDeviceInfo(CL_DEVICE_ENDIAN_LITTLE)

    for (var i = 0; i < NVALUES; ++i) {
        inputs.writeUInt32LE(i, i * BYTES_PER_ELEMENT); // inputs[offset]=i with offset=i*4 since each uint32 value takes 4 bytes
    }

    var source = fs.readFileSync(__dirname + "/square.cl").toString();

    var prog = cl.createProgramWithSource(ctx, source);

    cl.buildProgram(prog);

    var kern = cl.createKernel(prog, "square");

    var inputsMem = cl.createBuffer(ctx, cl.MEM_READ_ONLY | cl.MEM_COPY_HOST_PTR, NVALUES * BYTES_PER_ELEMENT, inputs);
    var outputsMem = cl.createBuffer(ctx, cl.MEM_WRITE_ONLY | cl.MEM_COPY_HOST_PTR, NVALUES * BYTES_PER_ELEMENT, outputs);

    cl.setKernelArg(kern, 0, "uint*", inputsMem);
    cl.setKernelArg(kern, 1, "uint*", outputsMem);
    cl.setKernelArg(kern, 2, "uint", NVALUES);

    var device = cl.getContextInfo(ctx, cl.CONTEXT_DEVICES)[0];
    var cq;
    if (cl.createCommandQueueWithProperties !== undefined) {
        cq = cl.createCommandQueueWithProperties(ctx, device, []); // OpenCL 2
    } else {
        cq = cl.createCommandQueue(ctx, device, null); // OpenCL 1.x
    }

    cl.enqueueNDRangeKernel(cq, kern, 1, null, [NVALUES], null);

    cl.enqueueReadBuffer(cq, outputsMem, true, 0, NVALUES * BYTES_PER_ELEMENT, outputs); // should contains i^2 for i=0,...,10000-1

    cl.finish(cq);

    console.log("#elems in outputs: " + outputs.length);
    var last_value = outputs.readUInt32LE(BYTES_PER_ELEMENT * (NVALUES - 1));
    console.log("Last value is : " + last_value + " should be " + ((NVALUES - 1) * (NVALUES - 1)));
}

function Square_TypedArray() {
    console.log("Using TypedArrays");

    var ctx;
    if (cl.createContextFromType !== undefined) {
        ctx = cl.createContextFromType(
            [cl.CONTEXT_PLATFORM, cl.getPlatformIDs()[0]], cl.DEVICE_TYPE_ALL, null, null);
    }
    else {
        var platform = cl.getPlatformIDs()[0];
        ctx = cl.createContext(
            [cl.CONTEXT_PLATFORM, platform],
            [cl.getDeviceIDs(platform, cl.DEVICE_TYPE_ALL)[0]]);
    }

    var NVALUES = 100;
    var BYTES_PER_ELEMENT = Uint32Array.BYTES_PER_ELEMENT;

    var inputs = new Uint32Array(NVALUES);
    var outputs = new Uint32Array(NVALUES);
    outputs.fill(0);

    for (var i = 0; i < NVALUES; ++i) {
        inputs[i] = i;
    }

    var source = fs.readFileSync(__dirname + "/square.cl").toString();

    var prog = cl.createProgramWithSource(ctx, source);

    cl.buildProgram(prog);

    var kern = cl.createKernel(prog, "square");

    var inputsMem = cl.createBuffer(ctx, cl.MEM_READ_ONLY | cl.MEM_COPY_HOST_PTR, NVALUES * BYTES_PER_ELEMENT, inputs);
    var outputsMem = cl.createBuffer(ctx, cl.MEM_WRITE_ONLY | cl.MEM_COPY_HOST_PTR, NVALUES * BYTES_PER_ELEMENT, outputs);

    cl.setKernelArg(kern, 0, "uint*", inputsMem);
    cl.setKernelArg(kern, 1, "uint*", outputsMem);
    cl.setKernelArg(kern, 2, "uint", NVALUES);

    var device = cl.getContextInfo(ctx, cl.CONTEXT_DEVICES)[0];
    var cq;
    if (cl.createCommandQueueWithProperties !== undefined) {
        cq = cl.createCommandQueueWithProperties(ctx, device, []); // OpenCL 2
    } else {
        cq = cl.createCommandQueue(ctx, device, null); // OpenCL 1.x
    }

    cl.enqueueNDRangeKernel(cq, kern, 1, null, [NVALUES], null);

    cl.enqueueReadBuffer(cq, outputsMem, true, 0, NVALUES * BYTES_PER_ELEMENT, outputs); // should contains i^2 for i=0,...,10000-1

    cl.finish(cq);

    console.log("#elems in outputs: " + outputs.length);
    var last_value = outputs[NVALUES - 1];
    console.log("Last value is : " + last_value + " should be " + ((NVALUES - 1) * (NVALUES - 1)));
}

Square();

console.log();

Square_TypedArray();

console.log("\n== Program terminated ==");
