// TODO: Disable warnings related to deprecated apis
import bindings from 'bindings';

import { OpenCL_10 } from './opencl-10';
import { OpenCL_11 } from './opencl-11';
import { OpenCL_12 } from './opencl-12';
import { OpenCL_20 } from './opencl-20';
import { OpenCL_21 } from './opencl-21';
import { OpenCL_22 } from './opencl-22';

export type cl = OpenCL_10 | OpenCL_11 | OpenCL_12 | OpenCL_20 | OpenCL_21 | OpenCL_22;

const binding = bindings('opencl.node');

export const OpenCL: cl = binding;

// global.WebCLPlatform=cl.webCLPlatform=function (_) { this._ = _; };
// global.WebCLDevice=cl.webCLDevice=function (_) { this._ = _; };
// global.WebCLContext=cl.webCLContext=function (_) { this._ = _; };
// global.WebCLCommandQueue=cl.webCLCommandQueue=function (_) { this._ = _; };
// global.WebCLMemObject=cl.webCLMemObject=function (_) { this._ = _; };
// global.WebCLEvent=cl.webCLEvent=function (_) { this._ = _; };
// global.WebCLProgram=cl.webCLProgram=function (_) { this._ = _; };
// global.WebCLKernel=cl.webCLKernel=function (_) { this._ = _; };
// global.WebCLSampler=cl.webCLSampler=function (_) { this._ = _; };

process.on('SIGINT', function () {
    console.log("\nGracefully shutting down from SIGINT (Ctrl-C)");
    // some other closing procedures go here
    process.exit();
});

process.on('exit', function () {
    // this releases all allocated OpenCL objects
    // global.gc();
    OpenCL.releaseAll();
});
