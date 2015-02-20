var cl = require("../../lib/opencl");

require("./device_selection");

var Utils = {
  withContext: function (exec) {
    var platform = MAIN_PLATFORM_ID;

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

  withCQ: function (ctx, device, exec) {
    var cq;
    if (Utils.checkVersion("1.x")) {
      cq = cl.createCommandQueue(ctx, device, null);
    } else {
      cq = cl.createCommandQueueWithProperties(ctx, device, []);
    }

    exec(cq);
    cl.releaseCommandQueue(cq);
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
    } else if (v == "1.x") {
      return (cl.VERSION_1_1 || cl.VERSION_1_2) && !cl.VERSION_2_0;
    } else if (v == "2.0") {
      return cl.VERSION_1_1 && cl.VERSION_1_2 && cl.VERSION_2_0;
    } else if (v == "2.x") {
      return cl.VERSION_1_1 && cl.VERSION_1_2 && cl.VERSION_2_0;
    } else {
      console.error("Unknown version : '" + v + "'");
      return false;
    }
  }
};

module.exports = Utils;
