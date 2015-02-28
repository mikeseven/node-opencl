var cl = require("../../lib/opencl");
var os = require("os");

if (global.MAIN_DEVICE_ID !== null && global.MAIN_DEVICE_ID !== undefined) {
  return;
}

var devices = [];
var devicesDisplayName = [];
var platforms = [];

cl.getPlatformIDs().reverse().forEach(function (p) {
  var pDevices = cl.getDeviceIDs(p).reverse();
  var info = cl.getPlatformInfo(p, cl.PLATFORM_VERSION);
  devicesDisplayName = devicesDisplayName.concat(pDevices.map(function (d) {
      return info + " : " + cl.getDeviceInfo(d, cl.DEVICE_NAME);
  }));
  devices = devices.concat(pDevices);
  platforms = platforms.concat(pDevices.map(function () {
    return p;
  }));

});

var deviceIdx = 0;

for (var index in process.argv) {
  var str = process.argv[index];
  if (str.indexOf("--device") == 0) {
    deviceIdx = str.substr(9);
  }
}

global.MAIN_DEVICE_ID = devices[deviceIdx];
global.MAIN_PLATFORM_ID = platforms[deviceIdx];

console.log("\n-----------------------------");
console.log("AVAILABLE DEVICES :");

devices.forEach(function (d, idx) {
  var vendor = devicesDisplayName[idx];
  console.log(idx + ". " + vendor);
});


console.log("\nENABLED DEVICE :");

console.log("Currently using device " + deviceIdx + ":");
console.log(devicesDisplayName[deviceIdx]);
console.log("\nTo use another device, add --device=<idx> to your mocha call");

console.log("\n");
console.log("OS : " +  os.platform());

console.log("-----------------------------\n");
