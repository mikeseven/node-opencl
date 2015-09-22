/*
* @Author: mikael
* @Date:   2015-09-21 22:30:37
* @Last Modified by:   mikael
* @Last Modified time: 2015-09-21 23:18:13
*/

'use strict';

var cl = require("../lib/opencl");
var log=console.log;
var BUFFER_SIZE=10;

function VectorAdd() {
  var A=new Uint32Array(BUFFER_SIZE);
  var B=new Uint32Array(BUFFER_SIZE);
  var C=new Uint32Array(BUFFER_SIZE);

  for (var i = 0; i < BUFFER_SIZE; i++) {
    A[i] = i;
    B[i] = i * 2;
    C[i] = 10;
  }

  // create GPU context for this platform
  var context = cl.createContextFromType(
    [cl.CONTEXT_PLATFORM, cl.getPlatformIDs()[0]],
    cl.DEVICE_TYPE_GPU,
    null, null);

  var devices=cl.getContextInfo(context, cl.CONTEXT_DEVICES);
  var device=devices[0];

  log('using device: '+cl.getDeviceInfo(device, cl.DEVICE_VENDOR).trim()+
    ' '+cl.getDeviceInfo(device, cl.DEVICE_NAME));

  var kernelSourceCode = [
"__kernel void vadd(__global int *a, __global int *b, __global int *c, uint iNumElements) ",
"{                                                                           ",
"    size_t i =  get_global_id(0);                                           ",
"    if(i > iNumElements) return;                                            ",
"    c[i] = a[i] + b[i];                                                     ",
"}                                                                           "
].join("\n");

  //Create and program from source
  var program=cl.createProgramWithSource(context, kernelSourceCode);

  //Build program
  cl.buildProgram(program);

  var size=BUFFER_SIZE*Uint32Array.BYTES_PER_ELEMENT; // size in bytes

  //Create kernel object
  var kernel;
  try {
    kernel= cl.createKernel(program, "vadd");
  }
  catch(err) {
    console.log(cl.getProgramBuildInfo(program, device,cl.PROGRAM_BUILD_LOG));
  }

  // Create buffer for A and copy host contents
  var aBuffer = cl.createBuffer(context, cl.MEM_READ_ONLY | cl.MEM_USE_HOST_PTR, size, A);

  // Create buffer for B and copy host contents
  var bBuffer = cl.createBuffer(context, cl.MEM_READ_ONLY | cl.MEM_USE_HOST_PTR, size, B);

  // Create buffer for that uses the host ptr C
  var cBuffer = cl.createBuffer(context, cl.MEM_WRITE_ONLY | cl.MEM_USE_HOST_PTR, size, C);

  //Set kernel args
  cl.setKernelArg(kernel, 0, "uint*", aBuffer);
  cl.setKernelArg(kernel, 1, "uint*", bBuffer);
  cl.setKernelArg(kernel, 2, "uint*", cBuffer);
  cl.setKernelArg(kernel, 3, "uint", BUFFER_SIZE);

  // Create command queue
  var queue=cl.createCommandQueue(context, device, 0);

  // Execute the OpenCL kernel on the list
  // var localWS = [5]; // process one list at a time
  // var globalWS = [clu.roundUp(localWS, BUFFER_SIZE)]; // process entire list
  var localWS=null;
  var globalWS=[BUFFER_SIZE];

  log("Global work item size: " + globalWS);
  log("Local work item size: " + localWS);

  // Execute (enqueue) kernel
  /*cl.enqueueNDRangeKernel(queue, kernel, 1,
      null,
      globalWS,
      localWS);

  log("using enqueueMapBuffer");
  // Map cBuffer to host pointer. This enforces a sync with
  // the host backing space, remember we choose GPU device.
  var map=cl.enqueueMapBuffer(queue,
      cBuffer,      // cl buffer
      cl.TRUE,      // block
      cl.MAP_READ,  // flags
      0,            // offset
      size);        // size

  var output="after map C= ";
  for (var i = 0; i < BUFFER_SIZE; i++) {
    output += C[i] + ", ";
  }
  log(output);

  // we are now reading values as bytes, we need to cast it to the output type we want
  output = "output = ";
  for (var i = 0; i < size; i++) {
    output += map[i] + ", ";
  }
  log(output);

  cl.enqueueUnmapMemObject(queue, cBuffer, map);

  output="after unmap C= ";
  for (var i = 0; i < BUFFER_SIZE; i++) {
    output += C[i] + ", ";
  }
  log(output);

  cl.finish(queue); // Finish all the operations

  printResults(A,B,C);*/

  // cleanup
  // queue.release();
  // kernel.release();
  // program.release();
  // aBuffer.release();
  // bBuffer.release();
  // cBuffer.release();
  // context.release();
  cl.releaseAll();
}

function printResults(A,B,C) {
  //Print input vectors and result vector
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

console.log("\n== Main thread terminated ==");
