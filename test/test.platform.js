var cl = require('../lib/opencl'),
  should = require('chai').should(),
  util = require('util'),
  withContext = require("../lib/test_utils").withContext,
  assert = require("chai").assert,
  chai = require("chai"),
  log = console.log;

var testString = function(code) {
  var info = cl.getPlatformInfo(cl.getPlatformIDs()[0], code);
  assert.isString(info);
};

describe("Platform", function () {
  describe("#getPlatformIDs", function () {

    var f = cl.getPlatformIDs;

    it("should return platform ids", function () {
      var ids = f();
      assert.isArray(ids);
      assert.isAbove(ids.length, 0);
    });

  });


  describe("#getPlatformInfo", function () {

    it("should return CL_PLATFORM_PROFILE", function () {
      testString(cl.PLATFORM_PROFILE);
    });

    it("should return CL_PLATFORM_VERSION", function () {
      testString(cl.PLATFORM_VERSION);
    });

    it("should return CL_PLATFORM_NAME", function () {
      testString(cl.PLATFORM_NAME);
    });

    it("should return CL_PLATFORM_VENDOR", function () {
      testString(cl.PLATFORM_VENDOR);
    });

    it("should return CL_PLATFORM_EXTENSIONS", function () {
      testString(cl.PLATFORM_EXTENSIONS);
    });

    it("should throw cl.INVALID_VALUE when passed an invalid platform attribute", function () {
      cl.getPlatformInfo.bind(cl.getPlatformInfo, cl.getPlatformIDs()[0], -1)
        .should.throw(cl.INVALID_VALUE.message);
    });

  });

});
