var cl=require('../lib/opencl'),
    should=require('chai').should(),
    assert = require('chai').assert,
    util=require('util'),
    log=console.log;

describe("Context", function() {
  var platforms=cl.getPlatformIDs();
  var platform=platforms[0];
  var devices=cl.getDeviceIDs(platform);
  var device=null;
  for(var i=0;i<devices.length;i++) {
    if(cl.getDeviceInfo(devices[i],cl.DEVICE_VENDOR) != 'Intel') {
      device=devices[i];
      break;
    }
  }
  log('using device '+cl.getDeviceInfo(device,cl.DEVICE_VENDOR)+" "+cl.getDeviceInfo(device,cl.DEVICE_NAME));

  describe("#createContext", function() {
    it("should throw if devices = null",function() {
      cl.createContext.bind(cl.createContext,null, null, null, null).should.throw(cl.INVALID_DEVICE);
    })
    it("should create a context with default properties for a platform",function() {
      var properties= [
        cl.CONTEXT_PLATFORM, platform
      ];
      var ctx=cl.createContext(properties, devices, null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    })
  });
  describe("#createContextFromType", function() {

  });
  describe("#retainContext", function() {

  });
  describe("#releaseContext", function() {

  });
  describe("#getContextInfo", function() {

  });
})
