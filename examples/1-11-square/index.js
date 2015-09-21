var cl = require("../../lib/opencl");
var fs = require("fs");


var Square = function() {

  var ctx = cl.createContextFromType(
    [cl.CONTEXT_PLATFORM, cl.getPlatformIDs()[0]], cl.DEVICE_TYPE_ALL, null, null);

  var NVALUES = 100;
  var inputs = new Buffer(NVALUES * 4); // *4 because uint is 4 bytes.
  var outputs = new Buffer(NVALUES * 4);

  // Note: using little endian for Intel-based machines, GPU follows same convention
  // as CPU typically but it should be detected with clGetDeviceInfo(CL_DEVICE_ENDIAN_LITTLE)

  for (var i = 0; i < NVALUES; ++ i) {
    inputs.writeUInt32LE(i, i * 4); // inputs[offset]=i with offset=i*4 since each uint32 value takes 4 bytes
  }

  var source = fs.readFileSync(__dirname + "/square.cl").toString();

  var prog = cl.createProgramWithSource(ctx, source);

  cl.buildProgram(prog);

  var kern = cl.createKernel(prog, "square");

  var inputsMem = cl.createBuffer(ctx, cl.MEM_COPY_HOST_PTR, NVALUES * 4, inputs);
  var outputsMem = cl.createBuffer(ctx, cl.MEM_COPY_HOST_PTR, NVALUES * 4, outputs);

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

  cl.enqueueReadBuffer(cq, outputsMem, true, 0, NVALUES * 4, outputs); // should contains i^2 for i=0,...,10000-1

  cl.finish(cq);

  var last_value=outputs.readUInt32LE(4*(NVALUES-1));
  console.log("Last value is : " + last_value + " should be " + ((NVALUES-1)*(NVALUES-1)));

  // manual cleanup (it is done automatically too)
  /*cl.releaseCommandQueue(cq);
  cl.releaseKernel(kern);
  cl.releaseProgram(prog);
  cl.releaseMemObject(inputsMem);
  cl.releaseMemObject(outputsMem);
  cl.releaseContext(ctx);*/
};


Square();
console.log("\n== Program terminated ==");
process.exit();
