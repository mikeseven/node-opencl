/*
* @Author: mikael
* @Date:   2015-09-21 18:06:21
* @Last Modified by:   mikael
* @Last Modified time: 2015-09-22 10:30:46
*/

'use strict';

var cl = require("../lib/opencl");
var log=console.log;

var BUFFER_SIZE=10;

function VectorAdd() {
  var A=new Uint32Array(BUFFER_SIZE);
  var B=new Uint32Array(BUFFER_SIZE);

  for (var i = 0; i < BUFFER_SIZE; i++) {
    A[i] = i;
    B[i] = i * 2;
  }

  var context = cl.createContextFromType(
    [cl.CONTEXT_PLATFORM, cl.getPlatformIDs()[0]],
    cl.DEVICE_TYPE_ALL,
    null, null);

  var kernelSourceCode = [
"__kernel void vadd(__global int *a, __global int *b, __global int *c, uint iNumElements) ",
"{                                                                           ",
"    size_t i =  get_global_id(0);                                           ",
"    if(i >= iNumElements) return;                                           ",
"    c[i] = a[i] + b[i];                                                     ",
"}                                                                           "
].join("\n");

  //Create and program from source
  var program=cl.createProgramWithSource(context, kernelSourceCode);

  //Build program
  cl.buildProgram(program);

  var size=BUFFER_SIZE*Uint32Array.BYTES_PER_ELEMENT; // size in bytes

  // Create buffer for A and B and copy host contents
  var aBuffer = cl.createBuffer(context, cl.MEM_READ_ONLY, size);
  var bBuffer = cl.createBuffer(context, cl.MEM_READ_ONLY, size);

  // Create buffer for C to read results
  var cBuffer = cl.createBuffer(context, cl.MEM_WRITE_ONLY, size);

  var device = cl.getContextInfo(context, cl.CONTEXT_DEVICES)[0];

  // Create kernel object
  var kernel;
  try {
    kernel= cl.createKernel(program, "vadd");
  }
  catch(err) {
    console.log(cl.getProgramBuildInfo(program, device, cl.PROGRAM_BUILD_LOG));
  }

  // Set kernel args
  cl.setKernelArg(kernel, 0, "uint*", aBuffer);
  cl.setKernelArg(kernel, 1, "uint*", bBuffer);
  cl.setKernelArg(kernel, 2, "uint*", cBuffer);
  cl.setKernelArg(kernel, 3, "uint", BUFFER_SIZE);

  // Create command queue
  var queue=cl.createCommandQueue(context, device, 0);

  // Do the work
  cl.enqueueWriteBuffer (queue, aBuffer, true, 0, A.length*Uint32Array.BYTES_PER_ELEMENT, A);
  cl.enqueueWriteBuffer (queue, bBuffer, true, 0, B.length*Uint32Array.BYTES_PER_ELEMENT, B);

  // Execute (enqueue) kernel
  log("using enqueueNDRangeKernel");
  cl.enqueueNDRangeKernel(queue, kernel, 1,
      null,
      [BUFFER_SIZE],
      null);

  // get results and block while getting them
  var C=new Uint32Array(BUFFER_SIZE);
  cl.enqueueReadBuffer (queue, cBuffer, true, 0, C.length*Uint32Array.BYTES_PER_ELEMENT, C);

  // print results
  printResults(A,B,C);

  // cleanup
  // test release each CL object
  // queue.release();
  // kernel.release();
  // program.release();
  // aBuffer.release();
  // bBuffer.release();
  // cBuffer.release();
  // context.release();

  // test release all CL objects
  // cl.releaseAll();

  // if no manual cleanup specified, cl.releaseAll() is called at exit of program
}

function printResults(A,B,C) {
  // Print input vectors and result vector
  var output = "\nA = ";
  for (var i = 0; i < BUFFER_SIZE; i++) {
    output += A[i] + ", ";
  }
  output += "\nB = ";
  for (var i = 0; i < BUFFER_SIZE; i++) {
    output += B[i] + ", ";
  }
  output += "\nC = ";
  for (var i = 0; i < BUFFER_SIZE; i++) {
    output += C[i] + ", ";
  }

  log(output);
}

VectorAdd();

// Main thread will always finish before CL callbacks are finished.
// Calling process.exit() in the main thread would skip CL callbacks from executing
console.log("\n== Main thread terminated ==");
