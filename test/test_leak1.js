'use strict';

var cl = require("../lib/opencl");
var memwatch = require('memwatch-next');

process.on('exit', function dump() {
  memwatch.gc();
  console.info("Exit.");
});

memwatch.on('stats', function(stats) {
  console.info("mem: "+stats.estimated_base+" trend: "+(stats.usage_trend>0 ? '+' : (stats.usage_trend>0 ? '-' : '=')));
});

var i=0;

var platform=cl.getPlatformIDs()[0];
var properties=[cl.CONTEXT_PLATFORM, platform];
var ctx;
if (cl.createContextFromType !== undefined) {
  ctx = cl.createContextFromType(properties, cl.DEVICE_TYPE_GPU, null, null);
}
else {
  ctx = cl.createContext(
    properties,
    cl.getDeviceIDs(platform, cl.DEVICE_TYPE_GPU));
}

memwatch.gc();
while (i++ < 1000) {
  var ev = cl.createUserEvent(ctx);
  cl.releaseEvent(ev);
  memwatch.gc();
}
memwatch.gc();

// === another test ===
//
// function tick() {
//   global.gc();
//   var ev = cl.createUserEvent(ctx);
//   cl.releaseEvent(ev);
//   global.gc();
//   if(i++ < 1000) setTimeout(tick,10);
// };

// var ctx = cl.createContextFromType(
//  [cl.CONTEXT_PLATFORM, cl.getPlatformIDs()[0]], cl.DEVICE_TYPE_CPU, null, null);

// global.gc();
// tick();
