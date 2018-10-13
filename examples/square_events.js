var cl = require("../lib/opencl");
var fs = require("fs");

var bcallback = false;

var Square = function() {

  var ctx = cl.createContextFromType(
    [cl.CONTEXT_PLATFORM, cl.getPlatformIDs()[0]], cl.DEVICE_TYPE_ALL, null, null);

  var NVALUES = 100;
  var inputs = new Buffer(NVALUES * 4);
  var outputs = new Buffer(NVALUES * 4);

  for (var i = 0; i < NVALUES; ++ i) {
    inputs.writeUInt32LE(i, i * 4);
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
    cq = cl.createCommandQueueWithProperties(ctx, device, []);
  } else {
    cq = cl.createCommandQueue(ctx, device, null);
  }

  // WARNING: non standard
  // by using arg[7]=true, enqueueXXX() returns a user event

  // here returned user event is not used
  cl.enqueueNDRangeKernel(cq, kern, 1, null, [NVALUES], null, [], true);

  // here we use the returned user event to associate a callback that will be called from OpenCL
  // once read buffer is complete.
  var ev = cl.enqueueReadBuffer(cq, outputsMem, true, 0, NVALUES * 4, outputs, [], true);
  console.log('have event');
  console.log(ev);

  cl.setEventCallback(ev, cl.COMPLETE, function(ev2,estat,userData){
    console.log('event handler');
    console.log("\nLast value is : " + outputs.readUInt32LE(4*(NVALUES-1)));
    
    console.log({event: ev2,error_status: estat,userData});

    // now the program can end
    console.log("== CL callback thread terminated ==");
    bcallback = true;
  },{data:"hello"});

};


Square();

// Main thread will always finish before CL callbacks are finished.
// Calling process.exit() in the main thread would skip CL callbacks from executing

(function wait () {
   if (!bcallback) {
     setTimeout(wait, 1000);
   }
   else {
    console.log("\n== Main thread terminated ==");
   }
})();

