var cl = require("../../lib/opencl");
var os = require("os");

module.exports = {};
var EXCEPTIONS_COUNT = 0;

var ErrDescription = function(device){
  this._os = [];
  this._driver = [];
  this._gpu = [];
  this.message = null;

  var self = this;
  this.os = function(os) { self._os.push(os); return self; };
  this.driver = function(driver) { self._driver.push(driver); return self; };
  this.gpu = function(gpu) { self._gpu.push(gpu); return self; };
  this.because = function(message){ self.message = message; return self; };

  var isConcerned = function(){
    var platform = cl.getDeviceInfo(device, cl.DEVICE_PLATFORM);
    console.log(self._gpu.indexOf(cl.getDeviceInfo(device, cl.DEVICE_NAME)) !== -1);
    console.log(self._driver.indexOf(cl.getPlatformInfo(platform, cl.PLATFORM_VERSION)));
    return self._os.indexOf(os.platform()) !== -1 &&
      self._gpu.indexOf(cl.getDeviceInfo(device, cl.DEVICE_NAME)) !== -1 &&
      self._driver.indexOf(cl.getPlatformInfo(platform, cl.PLATFORM_VERSION)) !== -1;
  };

  this.should = function (f) {
    if (!isConcerned()) return self;
    try {
      f();
    } catch(e) {
      throw new Error("A known driver issue has not been triggered");
    }
    return self;
  };

  this.raise = function() {
    if (!isConcerned()) return self;
    EXCEPTIONS_COUNT++;
    throw new Error("Driver issue : " + this.message);
  };

};

module.exports.exceptionsCount = function () {
  return EXCEPTIONS_COUNT;
};

module.exports.exclude = function (device) {
  return new ErrDescription(device);
};
