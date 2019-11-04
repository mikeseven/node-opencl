/*
* @Author: mikael
* @Date:   2015-09-22 21:13:24
* @Last Modified by:   Mikael Bourges-Sevenier
* @Last Modified time: 2018-02-24 19:30:06
*/

'use strict';

var cl = require('../lib/opencl');
var log = console.log;

saxpy();

function get_event_exec_time(event)
{
  // times are 64-bit values in nanoseconds. They are returned as [hi,lo] a 2-integer array
  // here we use the lo parts since this example is unlikely to go beyond 2^31 nanseconds per event.
  var start_time = cl.getEventProfilingInfo (event, cl.PROFILING_COMMAND_START);
  var end_time=cl.getEventProfilingInfo (event, cl.PROFILING_COMMAND_END);

  return (end_time[1] - start_time[1]) * 1e-6; // report in millisecond (from nanoseconds)
}

function saxpy() {
  var VECTOR_SIZE = 512*1024;
  log("SAXPY with vector size: "+VECTOR_SIZE+" elements");

  var alpha = 2.0;
  var A=new Float32Array(VECTOR_SIZE),
      B=new Float32Array(VECTOR_SIZE),
      C=new Float32Array(VECTOR_SIZE);

  for(var i = 0; i < VECTOR_SIZE; i++)
  {
      A[i] = i;
      B[i] = (VECTOR_SIZE - i);
      C[i] = 0;
  }

  // create GPU context for this platform
  var context;
  if (cl.createContextFromType !== undefined) {
    context = cl.createContextFromType(
    [cl.CONTEXT_PLATFORM, cl.getPlatformIDs()[0]],
    cl.DEVICE_TYPE_GPU,
    null, null);
  }
  else {
    var platform=cl.getPlatformIDs()[0];
    var devices=cl.getDeviceIDs(platform, cl.DEVICE_TYPE_GPU);
    console.info("Found "+devices.length+" GPUs: ");
    var device=devices[0];
    for(var i=0;i<devices.length;i++) {
      var name=cl.getDeviceInfo(devices[i],cl.DEVICE_NAME);
      console.info("  Devices "+i+": "+name);
      if(name.indexOf("Intel")==-1) // prefer discrete GPU
        device=devices[i];
    }

    context = cl.createContext(
      [cl.CONTEXT_PLATFORM, platform],
      [device]);
  }

  var device = cl.getContextInfo(context, cl.CONTEXT_DEVICES)[0];
  log('using device: '+cl.getDeviceInfo(device, cl.DEVICE_NAME));

  // Create command queue
  var queue;
  if (cl.createCommandQueueWithProperties !== undefined) {
    queue = cl.createCommandQueueWithProperties(context, device, [
      cl.QUEUE_PROPERTIES, cl.QUEUE_PROFILING_ENABLE
    ]); // OpenCL 2
  } else {
    queue = cl.createCommandQueue(context, device, cl.QUEUE_PROFILING_ENABLE); // OpenCL 1.x
  }

  var saxpy_kernel_source = [
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

  //Create and program from source
  var program=cl.createProgramWithSource(context, saxpy_kernel_source);

  //Build program
  cl.buildProgram(program);

  var size=VECTOR_SIZE*Float32Array.BYTES_PER_ELEMENT; // size in bytes

  // Create buffer for A and B and copy host contents
  var aBuffer = cl.createBuffer(context, cl.MEM_READ_ONLY, size);
  var bBuffer = cl.createBuffer(context, cl.MEM_READ_ONLY, size);

  // Create buffer for C to read results
  var cBuffer = cl.createBuffer(context, cl.MEM_WRITE_ONLY, size);

  // Create kernel object
  var kernel;
  try {
    kernel= cl.createKernel(program, "saxpy_kernel");
  }
  catch(err) {
    console.log(program.getBuildInfo(device,cl.PROGRAM_BUILD_LOG));
  }

  // Set kernel args
  cl.setKernelArg(kernel, 0, "float", alpha);
  cl.setKernelArg(kernel, 1, "float*", aBuffer);
  cl.setKernelArg(kernel, 2, "float*", bBuffer);
  cl.setKernelArg(kernel, 3, "float*", cBuffer);

  // Do the work
  var write_events=[];
  write_events[0]=cl.enqueueWriteBuffer (queue, aBuffer, false, 0, size, A, null, true);
  write_events[1]=cl.enqueueWriteBuffer (queue, bBuffer, false, 0, size, B, null, true);

  // Execute (enqueue) kernel
  var localWS = null; // process one list at a time
  var globalWS = [VECTOR_SIZE]; // process entire list

  var kernel_event=cl.enqueueNDRangeKernel(queue, kernel, 1, null, globalWS, localWS, write_events, true);

  // get results and block while getting them
  var read_event=cl.enqueueReadBuffer (queue, cBuffer, false, 0, size, C, [kernel_event], true);

  cl.waitForEvents([read_event]);
  // cl.finish(queue);
  log("C[last_value]="+C[VECTOR_SIZE-1]+" should be "+(2*(VECTOR_SIZE-1)+1));

  // get all event statistics
  log("Time to transfer matrix A: "+get_event_exec_time(write_events[0])+" ms");
  log("Time to transfer matrix B: "+get_event_exec_time(write_events[1])+" ms");
  log("Time to execute SAXPY kernel: "+get_event_exec_time(kernel_event)+" ms");
  log("Time to read matrix C: "+get_event_exec_time(read_event)+" ms");
}

