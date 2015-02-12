var cl=require('../lib/opencl'),
    testUtils=require('../lib/test_utils'),
    should=require('chai').should(),
    util=require('util'),
    assert = require('chai').assert,
    log=console.log;

describe("Sampler", function() {

  describe("#createSamplerWithProperties", function () {

    it("should return a sampler if given no properties", function () {
      testUtils.withContext(function(ctx){
        var sm = cl.createSamplerWithProperties(ctx, []);
        assert.isNotNull(sm);
        assert.isDefined(sm);

        console.log(cl.getSamplerInfo(sm, cl.SAMPLER_REFERENCE_COUNT));
      });
    });

  });


  describe("#createSampler", function() {
    console.log(cl.VERSION);
  });
  describe("#retainSampler", function() {

  });
  describe("#releaseSampler", function() {

  });
  describe("#getSamplerInfo", function() {

  });
})
