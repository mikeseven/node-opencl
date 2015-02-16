var cl=require('../lib/opencl'),
  util=require('util'),
  testUtils = require("../lib/test_utils"),
  log=console.log;

var platforms=cl.getPlatformIDs();
var platform=platforms[0];
var devices=cl.getDeviceIDs(platform).reverse();


// deviceId as arg
var deviceIdx = 0;

for (var index in process.argv) {
  var str = process.argv[index];
  if (str.indexOf("--device") == 0) {
    deviceIdx = str.substr(9);
  }
}

global.MAIN_DEVICE_ID = devices[deviceIdx];

console.log("\n-----------------------------");
console.log("AVAILABLE DEVICES :");

devices.forEach(function (d, idx) {
  var vendor = cl.getDeviceInfo(d, cl.DEVICE_NAME);
  console.log(idx + ". " + vendor);
});


console.log("\nENABLED DEVICE :");

console.log("Currently using device " + deviceIdx + ":");
console.log(cl.getDeviceInfo(global.MAIN_DEVICE_ID, cl.DEVICE_NAME));
console.log("\nTo use another device, add --device=<idx> to your mocha call");

console.log("-----------------------------\n");
