var cl = require('../lib/opencl'),
  should = require('chai').should(),
  util = require('util'),
  withContext = require("../lib/test_utils").withContext,
  assert = require("chai").assert,
  chai = require("chai"),
  log = console.log;

describe("Sampler", function () {
  describe("#createSampler", function () {

    var f = cl.createSampler;

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

  });

  describe("#retainSampler", function () {

    var f = cl.retainSampler;

    it("should increase the reference count when passed a valid argument", function () {
      withContext(function (context) {
        var sampler = cl.createSampler(context, cl.TRUE, cl.ADDRESS_NONE, cl.FILTER_LINEAR);
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
        var sampler = cl.createSampler(context, cl.TRUE, cl.ADDRESS_NONE, cl.FILTER_LINEAR);
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
        var sampler = cl.createSampler(context, cl.TRUE, cl.ADDRESS_NONE, cl.FILTER_LINEAR);
        var ret = f(sampler, cl.SAMPLER_REFERENCE_COUNT);

        assert.isNumber(ret);
        assert.strictEqual(1, ret);

        cl.releaseSampler(sampler);
      })
    });

    it("should return CL_SAMPLER_NORMALIZED_COORDS", function() {
      withContext(function(context) {
        var sampler = cl.createSampler(context, cl.TRUE, cl.ADDRESS_NONE, cl.FILTER_LINEAR);
        var ret = f(sampler, cl.SAMPLER_NORMALIZED_COORDS);

        assert.isBoolean(ret);
        assert.isTrue(ret);

        cl.releaseSampler(sampler);
      })
    });

    it("should return CL_SAMPLER_ADDRESSING_MODE", function() {
      withContext(function(context) {
        var sampler = cl.createSampler(context, cl.TRUE, cl.ADDRESS_NONE, cl.FILTER_LINEAR);
        var ret = f(sampler, cl.SAMPLER_ADDRESSING_MODE);

        assert.isNumber(ret);
        assert.strictEqual(ret, cl.ADDRESS_NONE);

        cl.releaseSampler(sampler);
      })
    });

    it("should return CL_SAMPLER_FILTER_MODE", function() {
      withContext(function(context) {
        var sampler = cl.createSampler(context, cl.TRUE, cl.ADDRESS_NONE, cl.FILTER_LINEAR);
        var ret = f(sampler, cl.SAMPLER_FILTER_MODE);

        assert.isNumber(ret);
        assert.strictEqual(ret, cl.FILTER_LINEAR);

        cl.releaseSampler(sampler);
      })
    });

  });
});
