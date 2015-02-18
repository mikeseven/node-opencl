var cl = require('../lib/opencl'),
  should = require('chai').should(),
  util = require('util'),
  withContext = require("../lib/test_utils").withContext,
  checkVersion = require("../lib/test_utils").checkVersion,
  assert = require("chai").assert,
  chai = require("chai"),
  log = console.log;

require("../lib/test_utils").initMainDevice();

var makeSampler = function (context) {
  if (checkVersion("1.*")) {
    return cl.createSampler(context, cl.TRUE, cl.ADDRESS_NONE, cl.FILTER_LINEAR);
  } else {
    return cl.createSamplerWithProperties(context,
      [cl.SAMPLER_NORMALIZED_COORDS, true,
        cl.SAMPLER_ADDRESSING_MODE, cl.ADDRESS_NONE,
        cl.SAMPLER_FILTER_MODE, cl.FILTER_LINEAR]);
  }
};


describe("Sampler", function () {
  describe("#createSampler", function () {

    var f = cl.createSampler;

    if (checkVersion("2.*")) {
      it("should be undefined as createSampler does not exist in OpenCL 2.0", function () {
        assert(f === undefined);
      });
    } else {
      it("should create a sampler when passed valid arguments", function () {
        withContext(function (context) {
          var sampler = f(context, cl.TRUE, cl.ADDRESS_NONE, cl.FILTER_LINEAR);
          assert.isObject(sampler);
          assert.isNotNull(sampler);
          cl.releaseSampler(sampler);
        });
      });

      it("should throw cl.INVALID_CONTEXT when passed an invalid context", function () {
        f.bind(f, null, cl.TRUE, cl.ADDRESS_NONE, cl.FILTER_LINEAR).should.throw(cl.INVALID_CONTEXT.message);
      });
    }
  });

  describe("#createSamplerWithProperties", function () {
    var f = cl.createSamplerWithProperties;

    if (checkVersion("1.*")) {
      it("should be undefined as createSamplerWithProperties does not exist in OpenCL 2.x", function () {
        assert(f === undefined);
      });
    } else {

      it("should throw cl.INVALID_CONTEXT when passed an invalid context", function () {
        f.bind(f, null, []).should.throw(cl.INVALID_CONTEXT.message);
      });

      it("should throw cl.INVALID_VALUE when passed an invalid property", function () {
        withContext(function(ctx) {
          f.bind(f, ctx, [cl.SAMPLER_FILTER_MODE, "salut"]).should.throw(cl.INVALID_VALUE.message);
        });
      });

      it("should return a sampler if given no properties", function () {
        withContext(function(ctx){
          var sm = cl.createSamplerWithProperties(ctx, []);
          assert.isObject(sm);

          cl.releaseSampler(sm);
        });
      });

      it("should return a sampler when given properties", function () {
        withContext(function(ctx){
          var sm = cl.createSamplerWithProperties(ctx,
            [cl.SAMPLER_NORMALIZED_COORDS, true,
              cl.SAMPLER_ADDRESSING_MODE, cl.ADDRESS_NONE,
              cl.SAMPLER_FILTER_MODE, cl.FILTER_LINEAR]);

          assert.isObject(sm);
          assert(cl.getSamplerInfo(sm, cl.SAMPLER_NORMALIZED_COORDS) == true);
          assert(cl.getSamplerInfo(sm, cl.SAMPLER_ADDRESSING_MODE) == cl.ADDRESS_NONE);
          assert(cl.getSamplerInfo(sm, cl.SAMPLER_FILTER_MODE) == cl.FILTER_LINEAR);

          cl.releaseSampler(sm);
        });
      });
    }

  });

  describe("#retainSampler", function () {

    var f = cl.retainSampler;

    it("should increase the reference count when passed a valid argument", function () {

      withContext(function (context) {
        var sampler = makeSampler(context);
        var count = cl.getSamplerInfo(sampler, cl.SAMPLER_REFERENCE_COUNT);
        assert.isNumber(count);
        assert.strictEqual(count, 1);
        f(sampler);

        count = cl.getSamplerInfo(sampler, cl.SAMPLER_REFERENCE_COUNT);
        assert.isNumber(count);
        assert.strictEqual(count, 2);

        cl.releaseSampler(sampler);
        cl.releaseSampler(sampler);
      })
    });

  });

  describe("#releaseSampler", function () {

    var f = cl.releaseSampler;

    it("should decrease reference count when passed a valid argument", function () {
      withContext(function (context) {
        var sampler = makeSampler(context);
        var count = cl.getSamplerInfo(sampler, cl.SAMPLER_REFERENCE_COUNT);
        assert.isNumber(count);
        assert.strictEqual(count, 1);
        f(sampler);
      });
    });

  });

  describe("#getSamplerInfo", function () {

    var f = cl.getSamplerInfo;

    it("should return CL_SAMPLER_REFERENCE_COUNT", function() {
      withContext(function(context) {
        var sampler = makeSampler(context);
        var ret = f(sampler, cl.SAMPLER_REFERENCE_COUNT);

        assert.isNumber(ret);
        assert.strictEqual(1, ret);

        cl.releaseSampler(sampler);
      })
    });

    it("should return CL_SAMPLER_NORMALIZED_COORDS", function() {
      withContext(function(context) {
        var sampler = makeSampler(context);
        var ret = f(sampler, cl.SAMPLER_NORMALIZED_COORDS);

        assert.isBoolean(ret);
        assert.isTrue(ret);

        cl.releaseSampler(sampler);
      })
    });

    it("should return CL_SAMPLER_ADDRESSING_MODE", function() {
      withContext(function(context) {
        var sampler = makeSampler(context);
        var ret = f(sampler, cl.SAMPLER_ADDRESSING_MODE);

        assert.isNumber(ret);
        assert.strictEqual(ret, cl.ADDRESS_NONE);

        cl.releaseSampler(sampler);
      })
    });

    it("should return CL_SAMPLER_FILTER_MODE", function() {
      withContext(function(context) {
        var sampler = makeSampler(context);
        var ret = f(sampler, cl.SAMPLER_FILTER_MODE);

        assert.isNumber(ret);
        assert.strictEqual(ret, cl.FILTER_LINEAR);

        cl.releaseSampler(sampler);
      })
    });

  });
});
