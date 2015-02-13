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
    });

    it("should return a device even if properties are null", function () {
      var ctx=cl.createContext(null, devices, null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });
  });

  describe("#createContextFromType", function() {

    it("should throw if type is unknown", function () {
      var properties= [
        cl.CONTEXT_PLATFORM, platform
      ];
      cl.createContextFromType.bind(cl.createContextFromType, properties, 0, null, null).should.throw(cl.INVALID_DEVICE);
    });

    it("should create a context with a wildcard type", function () {
      var properties= [
        cl.CONTEXT_PLATFORM, platform
      ];
      var ctx=cl.createContextFromType(properties, cl.DEVICE_TYPE_ALL, null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });

  });

  describe("#getContextInfo", function() {
    var properties= [
      cl.CONTEXT_PLATFORM, platform
    ];
    var ctx=cl.createContextFromType(properties, cl.DEVICE_TYPE_ALL, null, null);

    it("should return values with good types", function () {
      cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT).should.be.a.integer;
      cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT).should.be.a.integer;
      cl.getContextInfo(ctx, cl.CONTEXT_DEVICES).should.be.a.array;
      cl.getContextInfo(ctx, cl.CONTEXT_PROPERTIES).should.be.a.array;
    });

    it("should throw if an unknown param is given", function () {
      cl.getContextInfo.bind(cl.getContextInfo, ctx, 0)
        .should.throw(cl.INVALID_VALUE);
    });

    it("should have a reference count of 1", function () {
      assert(cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT) == 1);
    });

  });

  describe("#retainContext", function() {
    var properties= [
      cl.CONTEXT_PLATFORM, platform
    ];
    var ctx=cl.createContextFromType(properties, cl.DEVICE_TYPE_ALL, null, null);

    it("should have incremented ref count after call", function () {
      var before = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);
      cl.retainContext(ctx);
      var after = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);;
      assert(before + 1 == after);
    });
  });

  describe("#releaseContext", function() {
    var properties= [
      cl.CONTEXT_PLATFORM, platform
    ];
    var ctx=cl.createContextFromType(properties, cl.DEVICE_TYPE_ALL, null, null);

    it("should have decremented ref count after call", function () {

      var before = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);
      cl.retainContext(ctx);
      cl.releaseContext(ctx);
      var after = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);;
      assert(before  == after);
    });
  });
})
