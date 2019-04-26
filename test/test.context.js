var cl = require('../lib/opencl');
var should = require('chai').should();
var assert = require('chai').assert;
var util = require('util');
var U = require("./utils/utils");
var log = console.log;
var versions = require("./utils/versions");
var skip = require("./utils/diagnostic");

describe("Context", function () {

  var platform = global.MAIN_PLATFORM_ID;
  var properties = [cl.CONTEXT_PLATFORM, platform];
  var devices = [cl.getDeviceIDs(platform)[global.MAIN_DEVICE_IDX]];

  versions(["1.2"]).describe("#createContext", function () {

    skip().vendor("Apple").it("should throw if devices = null", function () {
      ex = cl.INVALID_VALUE.message;
      cl.createContext.bind(cl.createContext, null, null, null, null)
        .should.throw(ex);
    });

    it("should create a context with default properties for a platform", function () {
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
      U.bind(cl.createContextFromType, properties, 0, null, null)
        .should.throw(cl.INVALID_DEVICE_TYPE.message);
    });

    it("should create a context with a wildcard type", function () {
      var ctx = cl.createContextFromType(properties, cl.DEVICE_TYPE_ALL, null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });

    skip().vendor('nVidia').it("should create a context with the host processor type", function () {
      var ctx = cl.createContextFromType(properties, cl.DEVICE_TYPE_CPU, null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });

    skip().vendor('Intel').it("should create a context with the host GPU type", function () {
      var ctx = cl.createContextFromType(properties, cl.DEVICE_TYPE_GPU, null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });

    it.skip("should create a context with the accelerator type", function () {
      var ctx = cl.createContextFromType(properties, cl.DEVICE_TYPE_ACCELERATOR, null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });

    it.skip("should create a context with the custom type", function () {
      var ctx = cl.createContextFromType(properties, cl.DEVICE_TYPE_CUSTOM, null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });

    it("should create a context with the default type", function () {
      var ctx = cl.createContextFromType(properties, cl.DEVICE_TYPE_DEFAULT, null, null);
      assert.isNotNull(ctx);
      assert.isDefined(ctx);
      cl.releaseContext(ctx);
    });

  });

  describe("#getContextInfo", function () {

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

    it("should return at least one device", function () {
      var ctx = U.newContext({ type: cl.DEVICE_TYPE_ALL });
      var devices = cl.getContextInfo(ctx, cl.CONTEXT_DEVICES);
      assert(devices.length >= 1);
      assert.isObject(devices[0]);
      cl.releaseContext(ctx);
    });

    it("should throw cl.INVALID_VALUE if an unknown param is given", function () {
      var ctx = U.newContext({ type: cl.DEVICE_TYPE_ALL });
      cl.getContextInfo.bind(cl.getContextInfo, ctx, -1)
        .should.throw(cl.INVALID_VALUE.message);
      cl.releaseContext(ctx);
    });

    it("should have a reference count of 1", function () {
      var ctx = U.newContext({ type: cl.DEVICE_TYPE_ALL });
      assert(cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT) == 1);
      cl.releaseContext(ctx);
    });

  });

  describe("#retainContext", function () {
    it("should have incremented ref count after call", function () {
      var ctx = U.newContext({ type: cl.DEVICE_TYPE_ALL });
      var before = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);
      cl.retainContext(ctx);
      var after = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);
      assert(before + 1 == after);
      cl.releaseContext(ctx);
      cl.releaseContext(ctx);
    });
  });

  describe("#releaseContext", function () {
    it("should have decremented ref count after call", function () {
      var ctx = U.newContext({ type: cl.DEVICE_TYPE_ALL });
      var before = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);
      cl.retainContext(ctx);
      cl.releaseContext(ctx);
      var after = cl.getContextInfo(ctx, cl.CONTEXT_REFERENCE_COUNT);
      assert(before == after);
      cl.releaseContext(ctx);
    });
  });
});
