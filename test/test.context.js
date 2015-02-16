var cl=require('../lib/opencl'),
    should=require('chai').should(),
    assert = require('chai').assert,
    util=require('util'),
    testUtils = require("../lib/test_utils"),
    log=console.log;

describe("Context", function() {

  var platforms=cl.getPlatformIDs();
  var platform=platforms[0];
  var devices=cl.getDeviceIDs(platform);

  var device=devices[global.MAIN_DEVICE_IDX];

  describe("#createContext", function() {

    it("should throw if devices = null",function() {
      // DRIVER ISSUE : OSX
      var ex = testUtils.checkImplementation() == "osx" ?
        cl.INVALID_DEVICE.message : cl.INVALID_VALUE.message;

      cl.createContext.bind(cl.createContext,null, null, null, null)
        .should.throw(ex);
    });

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
      var ctx = cl.createContext(null, devices, null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });
  });

  describe("#createContextFromType", function() {

    if (testUtils.checkImplementation() == "osx") {
      it("[DRIVER ISSUE = OSX] should throw cl.INVALID_VALUE if type is unknown", function () {
        var properties= [
          cl.CONTEXT_PLATFORM, platform
        ];

        testUtils.bind(cl.createContextFromType, properties, 0, null, null)
          .should.throw(cl.INVALID_VALUE.message);
      });
    } else {
      it("should throw cl.DEVICE_NOT_FOUND if type is unknown", function () {
        // DRIVER ISSUE : OSX
        var ex = testUtils.checkImplementation() == "osx" ?
          cl.INVALID_VALUE.message : cl.DEVICE_NOT_FOUND.message;

        var properties= [
          cl.CONTEXT_PLATFORM, platform
        ];

        testUtils.bind(cl.createContextFromType, properties, 0, null, null)
          .should.throw(cl.DEVICE_NOT_FOUND.message);
      });
    }

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

    var testForType = function(clKey, _assert) {
      it("should return the good type for " + clKey, function () {
        testUtils.withContext(function (ctx) {
          var val = cl.getContextInfo(ctx, cl[clKey]);
          _assert(val);
          console.log(clKey +" = " + val);
        })
      })
    };

    testForType("CONTEXT_REFERENCE_COUNT", assert.isNumber.bind(assert));
    testForType("CONTEXT_DEVICES", assert.isArray.bind(assert));
    testForType("CONTEXT_PROPERTIES", assert.isArray.bind(assert));

    var ctx=cl.createContextFromType(properties, cl.DEVICE_TYPE_ALL, null, null);

    it("should throw cl.INVALID_VALUE if an unknown param is given", function () {
      cl.getContextInfo.bind(cl.getContextInfo, ctx, -1)
        .should.throw(cl.INVALID_VALUE.message);
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
      var after = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);
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
      var after = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);
      assert(before  == after);
    });
  });
});
