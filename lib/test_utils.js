var cl = require("./opencl");


module.exports = {

  initMainDevice: function () {
    if (global.MAIN_DEVICE_ID !== null && global.MAIN_DEVICE_ID !== undefined) {
      return;
    }

    var platforms=cl.getPlatformIDs();
    var platform=platforms[0];
    var devices=cl.getDeviceIDs(platform).reverse();

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
  },

  withContext: function (exec) {
    var platforms=cl.getPlatformIDs();
    var platform=platforms[0];

    var properties= [
      cl.CONTEXT_PLATFORM, platform
    ];
    var device=global.MAIN_DEVICE_ID;

    var ctx=cl.createContext(properties, [device], null, null);
    exec(ctx, device, platform);

    cl.releaseContext(ctx);
  },

  withProgram: function (ctx, source, exec) {
    var prg = cl.createProgramWithSource(ctx, source);
    var ret = cl.buildProgram(prg, undefined, "-cl-kernel-arg-info");

    exec(prg);

    cl.releaseProgram(prg);
  },

  bind : function(/*...*/) {
    var args = Array.prototype.slice.call(arguments);
    var fct = args.shift();
    return function(){
        return fct.apply(fct, args);
    };
  },

  checkImplementation : function() {
    // We certainly need a better implementation of this method
    if (!cl.PLATFORM_ICD_SUFFIX_KHR) {
      return "osx";
    } else {
      return "other";
    }
  },

  checkVersion : function(v) {
    if (v == "1.1") {
      return cl.VERSION_1_1 && !cl.VERSION_1_2 && !cl.VERSION_2_0;
    } else if (v == "1.2") {
      return cl.VERSION_1_1 && cl.VERSION_1_2 && !cl.VERSION_2_0
    } else if (v == "1.*") {
      return (cl.VERSION_1_1 || cl.VERSION_1_2) && !cl.VERSION_2_0;
    } else if (v == "2.0") {
      return cl.VERSION_1_1 && cl.VERSION_1_2 && cl.VERSION_2_0;
    } else if (v == "2.*") {
      return cl.VERSION_1_1 && cl.VERSION_1_2 && cl.VERSION_2_0;
    } else {
      console.error("Unknown version : " + v);
      return false;
    }
  }

};
