var cl = require("../../lib/opencl");
var os = require("os");

var vendors = {
  "Advanced Micro Devices, Inc." : "AMD",
  "AMD" : "AMD",
  "Apple" : "Apple",
  "Intel" : "Intel",
  "Intel Inc." : "Intel",
  "Intel(R) Corporation": "Intel",
  "nVidia": "nVidia",
  "NVIDIA Corporation": "nVidia"
};

module.exports = function() {
  var checks = undefined;
  var _vendors = [];
  var _oss = [];
  var _devices = [];
  var platformVendor = vendors[cl.getPlatformInfo(global.MAIN_PLATFORM_ID, cl.PLATFORM_VENDOR)];
  var deviceVendor = vendors[cl.getDeviceInfo(global.MAIN_DEVICE_ID,cl.DEVICE_VENDOR)];
  var osName = os.platform();

  var match = function(){
    var vmatch = _vendors.length == 0 || _vendors.some(function (v) {
        return platformVendor == v;
    });

    var omatch = _oss.length == 0 || _oss.some(function (o) {
        return osName == o;
    });

    var dmatch = _devices.length == 0 || _devices.some(function (o) {
        return deviceVendor == o;
    });

    return vmatch && omatch && dmatch;
  };

  var obj = {
    vendor: function (...xs) {
      _vendors.push(...xs);
      checks |= xs.length;
      return obj;
    },

    device: function (...xs) {
      _devices.push(...xs);
      checks |= xs.length;
      return obj;
    },

    os : function (...xs) {
      _oss.push(...xs);
      checks |= xs.length;
      return obj;
    },

    it : function (desc) {
      if (checks === undefined || (checks > 0 && match())) {
        console.warn("Cancelling " + desc + " because of known driver issue");
        it.skip.apply(it, arguments);
      } else {
        it.apply(it, arguments);
      }
    }
  };

  return obj;
};

module.exports.vendors = vendors;
