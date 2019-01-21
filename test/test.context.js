var cl = require('../lib/opencl');
var should = require('chai').should();
var assert = require('chai').assert;
var util = require('util');
var U = require("./utils/utils");
var log = console.log;
var versions = require("./utils/versions");
var skip = require("./utils/diagnostic");

describe("Context", function () {

  var platforms = cl.getPlatformIDs();
  var platform = platforms[0];
  var devices = cl.getDeviceIDs(platform);

  var device = devices[global.MAIN_DEVICE_IDX];

  versions(["1.2"]).describe("#createContext", function () {

    skip().vendor("Apple").it("should throw if devices = null", function () {
      ex = cl.INVALID_VALUE.message;
      cl.createContext.bind(cl.createContext, null, null, null, null)
        .should.throw(ex);
    });

    it("should create a context with default properties for a platform", function () {
      var properties = [
        cl.CONTEXT_PLATFORM, platform
      ];
      var ctx = cl.createContext(properties, devices, null, null);
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

  versions(["2.0"]).describe("#createContextFromType", function () {

    skip().it("should throw cl.CL_INVALID_DEVICE_TYPE if type is unknown", function () {

      var ex = cl.INVALID_DEVICE_TYPE.message;

      var properties = [
        cl.CONTEXT_PLATFORM, platform
      ];

      U.bind(cl.createContextFromType, properties, 0, null, null)
        .should.throw(cl.INVALID_DEVICE_TYPE.message);
    });

    it("should create a context with a wildcard type", function () {
      var properties = [
        cl.CONTEXT_PLATFORM, platform
      ];
      var ctx = cl.createContextFromType(properties, [cl.DEVICE_TYPE_ALL], null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });

    it.skip("should create a context with the host processor type", function () {
      var properties = [
        cl.CONTEXT_PLATFORM, platform
      ];
      var ctx = cl.createContextFromType(properties, [cl.DEVICE_TYPE_CPU], null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });

    it("should create a context with the host GPU type", function () {
      var properties = [
        cl.CONTEXT_PLATFORM, platform
      ];
      var ctx = cl.createContextFromType(properties, [cl.DEVICE_TYPE_GPU], null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });

    it.skip("should create a context with the accelerator type", function () {
      var properties = [
        cl.CONTEXT_PLATFORM, platform
      ];
      var ctx = cl.createContextFromType(properties, [cl.DEVICE_TYPE_ACCELERATOR], null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });

    it.skip("should create a context with the custom type", function () {
      var properties = [
        cl.CONTEXT_PLATFORM, platform
      ];
      var ctx = cl.createContextFromType(properties, [cl.DEVICE_TYPE_CUSTOM], null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });

    it("should create a context with the default type", function () {
      var properties = [
        cl.CONTEXT_PLATFORM, platform
      ];
      var ctx = cl.createContextFromType(properties, [cl.DEVICE_TYPE_DEFAULT], null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });

  });

  describe("#getContextInfo", function () {
    var properties = [
      cl.CONTEXT_PLATFORM, platform
    ];

    var testForType = function (clKey, _assert) {
      it("should return the good type for " + clKey, function () {
        U.withContext(function (ctx) {
          var val = cl.getContextInfo(ctx, cl[clKey]);
          _assert(val);
          console.log(clKey + " = " + val);
        })
      })
    };

    testForType("CONTEXT_REFERENCE_COUNT", assert.isNumber.bind(assert));
    testForType("CONTEXT_DEVICES", assert.isArray.bind(assert));
    testForType("CONTEXT_PROPERTIES", assert.isArray.bind(assert));
    testForType("CONTEXT_NUM_DEVICES", assert.isNumber.bind(assert));

    var ctx;
    if (cl.createContextFromType !== undefined) {
      ctx = cl.createContextFromType(properties, cl.DEVICE_TYPE_ALL, null, null);
    }
    else {
      ctx = cl.createContext(
        properties,
        cl.getDeviceIDs(platform, cl.DEVICE_TYPE_ALL));
    }

    it("should return at least one device", function () {
      var devices = cl.getContextInfo(ctx, cl.CONTEXT_DEVICES);
      assert(devices.length >= 1);
      assert.isObject(devices[0]);
    });

    it("should throw cl.INVALID_VALUE if an unknown param is given", function () {
      cl.getContextInfo.bind(cl.getContextInfo, ctx, -1)
        .should.throw(cl.INVALID_VALUE.message);
    });

    it("should have a reference count of 1", function () {
      assert(cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT) == 1);
    });

  });

  describe("#retainContext", function () {
    var properties = [
      cl.CONTEXT_PLATFORM, platform
    ];
    var ctx;
    if (cl.createContextFromType !== undefined) {
      ctx = cl.createContextFromType(properties, cl.DEVICE_TYPE_ALL, null, null);
    }
    else {
      ctx = cl.createContext(
        properties,
        cl.getDeviceIDs(platform, cl.DEVICE_TYPE_ALL));
    }

    it("should have incremented ref count after call", function () {
      var before = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);
      cl.retainContext(ctx);
      var after = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);
      assert(before + 1 == after);
    });
  });

  describe("#releaseContext", function () {
    var properties = [
      cl.CONTEXT_PLATFORM, platform
    ];
    var ctx;
    if (cl.createContextFromType !== undefined) {
      ctx = cl.createContextFromType(properties, cl.DEVICE_TYPE_ALL, null, null);
    }
    else {
      ctx = cl.createContext(
        properties,
        cl.getDeviceIDs(platform, cl.DEVICE_TYPE_ALL));
    }

    it("should have decremented ref count after call", function () {

      var before = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);
      cl.retainContext(ctx);
      cl.releaseContext(ctx);
      var after = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);
      assert(before == after);
    });
  });
});
