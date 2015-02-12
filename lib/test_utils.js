var cl = require("./opencl");

module.exports = {
  withContext: function (exec) {
    var platforms=cl.getPlatformIDs();
    var platform=platforms[0];
    var properties= [
      cl.CONTEXT_PLATFORM, platform
    ];
    var ctx=cl.createContextFromType(properties, cl.DEVICE_TYPE_ALL, null, null);
    exec(ctx);

    cl.releaseContext(ctx);
  }
};
