var cl = require("../../lib/opencl");
var fs = require("fs");

var Square = function() {

  var ctx = cl.createContextFromType(
    [cl.CONTEXT_PLATFORM, cl.getPlatformIDs()[0]], cl.DEVICE_TYPE_ALL, null, null);

  var inputs = new Buffer(10000 * 4);
  var outputs = new Buffer(10000 * 4);

  for (var i = 0; i < 10000; ++ i) {
    inputs.writeUInt32LE(i, i * 4);
  }

  var source = fs.readFileSync(__dirname + "/square.cl").toString();
    
  var prog = cl.createProgramWithSource(ctx, source);

  cl.buildProgram(prog);

  var kern = cl.createKernel(prog, "square");

  var inputsMem = cl.createBuffer(
    ctx, cl.MEM_COPY_HOST_PTR, 10000 * 4, inputs);
  var outputsMem = cl.createBuffer(
    ctx, cl.MEM_COPY_HOST_PTR, 10000 * 4, outputs);

  cl.setKernelArg(kern, 0, "uint*", inputsMem);
  cl.setKernelArg(kern, 1, "uint*", outputsMem);
  cl.setKernelArg(kern, 2, "uint", 10000);

  var device = cl.getContextInfo(ctx, cl.CONTEXT_DEVICES)[0];
  var cq;
  if (cl.createCommandQueueWithProperties !== undefined) {
    cq = cl.createCommandQueueWithProperties(ctx, device, []);
  } else {
    cq = cl.createCommandQueue(ctx, device, null);
  }
  
  cl.enqueueNDRangeKernel(
    cq, kern, 1, null, [10000], null,
    [], true);

  var ev = cl.enqueueReadBuffer(
    cq, outputsMem, true, 0, 10000 * 4, outputs,
    [], true);

  cl.setEventCallback(ev, cl.COMPLETE, function(){
    console.log("Last value is : " + outputs.readUInt32LE(40000 - 4));
  });

};


Square();
