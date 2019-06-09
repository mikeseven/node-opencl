import * as fs from 'fs';

import {
    OpenCL as cl,
    cl_mem,
    cl_context,
    cl_device_id,
    cl_command_queue,
    cl_kernel,
} from '..';

const count = 100;

class SquareExample {
    private source: string = fs.readFileSync(__dirname + "/square.cl").toString();
    private inBuffer: Buffer;
    private outBuffer: Buffer;

    private in: cl_mem;
    private out: cl_mem;
    private kernel: cl_kernel;
    private context: cl_context;
    private device: cl_device_id;
    private commandQueue: cl_command_queue;

    public constructor() {
        let platform = cl.getPlatformIDs()[0];
        let properties = [cl.CONTEXT_PLATFORM, platform];

        if (cl.createContextFromType !== undefined) {
            this.context = cl.createContextFromType(properties, cl.DEVICE_TYPE_ALL);
        } else {
            let device_ids = cl.getDeviceIDs(platform, cl.DEVICE_TYPE_ALL);
            this.context = cl.createContext(properties, device_ids);
        }

        this.inBuffer = Buffer.alloc(count * 4);
        this.outBuffer = Buffer.alloc(count * 4);

        for (let i = 0; i < count; ++i) {
            this.inBuffer.writeUInt32LE(i, i * 4);
        }

        const program = cl.createProgramWithSource(this.context, this.source);

        cl.buildProgram(program);

        try {
            this.kernel = cl.createKernel(program, "square");
        }
        catch (error) {
            let [device] = cl.getContextInfo(this.context, cl.CONTEXT_DEVICES);
            console.log(cl.getProgramBuildInfo(program, device, cl.PROGRAM_BUILD_LOG));

            throw error;
        }

        this.in = cl.createBuffer(this.context, cl.MEM_COPY_HOST_PTR, count * 4, this.inBuffer);
        this.out = cl.createBuffer(this.context, cl.MEM_COPY_HOST_PTR, count * 4, this.outBuffer);

        cl.setKernelArg(this.kernel, 0, "uint*", this.in);
        cl.setKernelArg(this.kernel, 1, "uint*", this.out);
        cl.setKernelArg(this.kernel, 2, "uint", count);

        this.device = cl.getContextInfo(this.context, cl.CONTEXT_DEVICES)[0];

        this.commandQueue = cl.v20
            ? cl.createCommandQueueWithProperties(this.context, this.device)
            : this.commandQueue = cl.createCommandQueue(this.context, this.device);
    }

    public run() {
        // here returned user event is not used
        cl.enqueueNDRangeKernel(this.commandQueue, this.kernel, 1, null, [count], null);

        // here we use the returned user event to associate a callback that will be called from OpenCL
        // once read(Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer) | (Buffer | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | ArrayBuffer)is complete.
        const event = cl.enqueueReadBuffer(this.commandQueue, this.out, true, 0, count * 4, this.outBuffer);

        if (cl.v12) {
            console.log("Using callbacks to wait for event completion");

            cl.setEventCallback(event, cl.COMPLETE, () => {
                console.log("\nLast value is : " + this.outBuffer.readUInt32LE(4 * (count - 1)));

                // now the program can end
                console.log("== CL callback thread terminated ==");
                process.exit();
            });
        } else {
            console.log("Not using callbacks to wait for event completion");
            console.log(event);
            console.log("\nLast value is : " + this.outBuffer.readUInt32LE(4 * (count - 1)));
        }
    }
}

let square = new SquareExample();

square.run();

// Main thread will always finish before CL callbacks are finished.
// Calling process.exit() in the main thread would skip CL callbacks from executing
