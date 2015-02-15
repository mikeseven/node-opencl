var cl = require("./opencl");

var WARNING_HAS_BEEN_DISPLAYED = false;

module.exports = {

  withContext: function (exec) {
    var platforms=cl.getPlatformIDs();
    var platform=platforms[0];
    var devices=cl.getDeviceIDs(platform);

    var properties= [
      cl.CONTEXT_PLATFORM, platform
    ];
    var device=null, fallback=null;
    for(var i=0;i<devices.length;i++) {
      if(cl.getDeviceInfo(devices[i],cl.DEVICE_VENDOR) == 'Intel') {
        fallback = devices[i];
      } else {
        device=devices[i];
        break;
      }
    }

    if (device == null && fallback == null) {
      throw new Error("No OpenCL compliant device found");
    }

    if (device == null && fallback != null) {
      device = fallback;

      if (!WARNING_HAS_BEEN_DISPLAYED) {
        console.warn("Using Intel device");
        WARNING_HAS_BEEN_DISPLAYED = true;
      }
    }

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
    return function(){
        return Function.apply.call(args);
    };
  }


};
