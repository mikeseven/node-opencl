'use strict';

var cl = require("../lib/opencl");
var memwatch = require('node-memwatch');

process.on('exit', function dump() {
  memwatch.gc();
  console.info("Exit.");
});

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

test();

function test() {

  var i = 0;

  memwatch.on('stats', logStats);

  loop();

  function loop() {
    memwatch.gc();
    cl.releaseEvent(cl.createUserEvent(ctx));
    memwatch.gc();
    if(++i < 1000) {
      setTimeout(loop,10);
    } else {
      memwatch.removeListener('stats', logStats);
      process.exit(0);
    }
  }

  function logStats(stats) {
    console.info("mem: "+
      (stats.estimated_base / (1024 ** 2)).toFixed(2) + "mb" +
      " trend: "+(stats.usage_trend>0 ? '+' : (stats.usage_trend<0 ? '-' : '=')));
  }
};
