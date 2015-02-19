var cl = require('../lib/opencl');
var should = require('chai').should();
var assert = require("chai").assert;
var util = require('util');
var log = console.log;
var U = require("./utils/utils");
var versions = require("./utils/versions");

var isValidCQ = function (cq) {
  assert.isNotNull(cq);
  assert.isDefined(cq);
};


describe("CommandQueue", function() {

  describe("#createCommandQueue", function() {

    var f = cl.createCommandQueue;

    versions(["2.x"]).hasUndefined(f);

    versions(["1.x"]).it("should create a valid command queue", function () {

      U.withContext(function(ctx, device){
        var cq = cl.createCommandQueue(ctx, device, null);
        isValidCQ(cq);
        cl.releaseCommandQueue(cq);
      });
    });

    versions(["1.x"]).it("should fail given an invalid property", function () {
      U.withContext(function(ctx, device) {
        U.bind(f,ctx, device, -1).should.throw(cl.INVALID_VALUE.message);
      });
    });

    versions(["1.x"]).it("should fail given an invalid device", function() {
      U.withContext(function(ctx, device){
        U.bind(f, ctx, "test", []).should.throw(cl.INVALID_DEVICE.message);
      });
    });

  });

  describe("#createCommandQueueWithProperties", function() {
    var f = cl.createCommandQueueWithProperties;

    versions(["1.x"]).hasUndefined(f);


    versions(["2.x"]).it("should create a valid command queue", function () {
      U.withContext(function(ctx, device){
        var cq = f(ctx, device, []);
        isValidCQ(cq);
        cl.releaseCommandQueue(cq);
      });
    });

    versions(["2.x"]).it("should fail given an invalid property", function () {
      U.withContext(function(ctx, device) {

        U.bind(f, ctx, device, ["TEST"])
          .should.throw(cl.INVALID_VALUE.message);

      });
    });

    versions(["2.x"]).it("should fail given an invalid device", function() {
      U.withContext(function(ctx, device){
        U.bind(f, ctx, "TEST", [])
          .should.throw(cl.INVALID_DEVICE.message);
      });
    });

  });

  describe("# ( TODO ) retainCommandQueue", function() {
    // FIXME
  });
  describe("# ( TODO ) releaseCommandQueue", function() {
    // FIXME
  });

  describe("# ( TODO ) getCommandQueueInfo", function() {
    // FIXME
  });

  describe("# ( TODO ) flush", function() {

    it("should return undefined", function () {
      // TODO
    });

  });


  describe("# ( TODO ) finish", function() {

    it("should return undefined", function () {
      // TODO
    });

  });

  describe("# ( TODO ) enqueueReadBuffer", function() {

    it("should work with read only buffers", function () {
      // TODO
    });

    it("should work with write buffers", function () {
      // TODO
    });


    it("should return general exceptions", function () {
      // TODO
    });

  });


  describe("# ( TODO ) enqueueReadBufferRect", function() {
    it("should work with read write buffers", function () {
      // TODO
    });

    it("should work with write buffers", function () {
      // TODO
    });

  });


  describe("# ( TODO ) enqueueWriteBuffer", function() {
    it("should work with read only buffers", function () {
      // TODO
    });

    it("should work with write buffers", function () {
      // TODO
    });

  });


  describe("# ( TODO ) enqueueWriteBufferRect", function() {
    it("should work with read write buffers", function () {
      // TODO
    });

    it("should work with write buffers", function () {
      // TODO
    });

  });

  describe("# ( TODO ) enqueueFillBuffer", function() {
    // FIXME
  });

  describe("# ( TODO ) enqueueCopyBuffer", function() {
    it("should work with read only buffers", function () {
      // TODO
    });

    it("should work with write buffers", function () {
      // TODO
    });

  });


  describe("# ( TODO ) enqueueCopyBufferRect", function() {
    it("should work with read only buffers", function () {
      // TODO
    });

    it("should work with write buffers", function () {
      // TODO
    });

    it("should work with different values of source and destination offsets", function () {
      // TODO
    });

    it("should throw if bad number of bytes", function () {
      // TODO
    });

    it("should <> on overlapping copies", function () {
      // TODO
    });

    it("should work with different buffer origin values", function () {
      // TODO
    });

    it("should work with different host origin values", function () {
      // TODO
    });

    it("should work with different region values", function () {
      // TODO
    });

    it("should work with different row pitch values", function () {
      // TODO
    });

    it("should work with different splice pitch values", function () {
      // TODO
    });

    it("should work with different host pointer values", function () {
      // TODO
    });
  });


  describe("# ( TODO ) enqueueReadImage", function() {
    it("should work with read write buffers", function () {
      // TODO
    });

    it("should work with write buffers", function () {
      // TODO
    });

    it("should work with read buffers", function () {
      // TODO
    });

    it("should fail with bad parameters", function () {
      // TODO
    });

    it("should fail on bad image size", function () {
      // TODO
    });

    it("should fail with bad row pitch values", function () {
      // TODO
    });

    it("should return general exceptions", function () {
      // TODO
    });

  });

  describe("# ( TODO ) enqueueWriteImage", function() {
    it("should work with read write images", function () {
      // TODO
    });

    it("should work with write images", function () {
      // TODO
    });

    it("should work with read images", function () {
      // TODO
    });

    it("should fail on bad image size", function () {
      // TODO
    });

    it("should fail with bad row pitch values", function () {
      // TODO
    });

    it("should return general exceptions", function () {
      // TODO
    });
  });


  describe("# ( TODO ) enqueueFillImage", function() {
    // FIXME
  });


  describe("# ( TODO ) enqueueCopyImage", function() {
    it("should work with read write images", function () {
      // TODO
    });

    it("should work with write images", function () {
      // TODO
    });

    it("should work with read images", function () {
      // TODO
    });
  });


  describe("# ( TODO ) enqueueCopyImageToBuffer", function() {
    it("should work with read only buffers", function () {
      // TODO
    });

    it("should work with write buffers", function () {
      // TODO
    });

    it("should work with different values of source and destination offsets", function () {
      // TODO
    });

    it("should throw if bad number of bytes", function () {
      // TODO
    });

    it("should <> on overlapping copies", function () {
      // TODO
    });

    it("should work with different buffer origin values", function () {
      // TODO
    });

    it("should work with different host origin values", function () {
      // TODO
    });

    it("should work with different region values", function () {
      // TODO
    });

    it("should work with different row pitch values", function () {
      // TODO
    });

    it("should work with different splice pitch values", function () {
      // TODO
    });

    it("should work with different host pointer values", function () {
      // TODO
    });

    it("should return general exceptions", function () {
      // TODO
    });
  });

  describe("# ( TODO ) enqueueCopyBufferToImage", function() {
    it("should work with read only buffers", function () {
      // TODO
    });

    it("should work with write buffers", function () {
      // TODO
    });

    it("should work with different values of origin", function () {
      // TODO
    });

    it("should work with different values of offset", function () {
      // TODO
    });

    it("should work with different values of region", function () {
      // TODO
    });
  });

  describe("# ( TODO ) enqueueMapBuffer", function() {
    // FIXME
  });

  describe("# ( TODO ) enqueueMapImage", function() {
    // FIXME
  });

  describe("# ( TODO ) enqueueMapBuffer", function() {
    // FIXME
  });

  describe("# ( TODO ) enqueueUnmapMemObject", function() {
    // FIXME
  });

  describe("# ( TODO ) enqueueMigrateMemObjects", function() {
    // FIXME
  });

  describe("# ( TODO ) enqueueNDRangeKernel", function() {

    it("should work with a valid call", function () {
      // TODO
    });

    it("should work for kernels with __attribute", function () {
      // TODO
    });

  });

  describe("# ( TODO ) enqueueTask", function() {
    // TODO
  });

  describe("# ( TODO ) enqueueNativeKernel", function() {
    // TODO
  });

  describe("# ( TODO ) enqueueMarkerWithWaitList", function() {
    // TODO
  });

  describe("# ( TODO ) enqueueBarrierWithWaitList", function() {
    // TODO
  });

});
