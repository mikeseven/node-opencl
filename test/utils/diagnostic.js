var cl = require("../../lib/opencl");
var os = require("os");

var vendors = {
  "Advanced Micro Devices, Inc." : "AMD",
  "Apple" : "Apple",
  "Intel" : "Intel",
  "nVidia": "nVidia"
};

module.exports = function() {
  var _vendors = [];
  var _oss = [];
  var platformVendor = vendors[cl.getPlatformInfo(global.MAIN_PLATFORM_ID, cl.PLATFORM_VENDOR)];
  var osName = os.platform();

  var match = function(){
    var vmatch = _vendors.length == 0 || _vendors.some(function (v) {
        return platformVendor == v;
    });

    var omatch = _oss.length == 0 || _oss.some(function (o) {
        return osName == o;
    });

    return vmatch && omatch;
  };

  var obj = {
    vendor: function (v) {
      _vendors.push(v);
      return obj;
    },

    os : function (o) {
      _oss.push(o);
      return obj;
    },

    it : function (desc) {
      if (match()) {
        console.warn("Cancelling " + desc + " because of known driver issue");
        it.skip.apply(it, arguments);
      } else {
        it.apply(it, arguments);
      }
    }
  };

  return obj;
};
