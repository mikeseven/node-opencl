var cl=require('../lib/opencl'),
    should=require('chai').should(),
    assert = require("chai").assert,
    util=require('util'),
    log=console.log,
    testUtils = require("../lib/test_utils");


var isValidCQ = function (cq) {
  assert.isNotNull(cq);
  assert.isDefined(cq);
};


describe("CommandQueue", function() {

  describe("#createCommandQueue", function() {
    it("should create a valid command queue", function () {
      testUtils.withContext(function(ctx, device){
        var cq = cl.createCommandQueue(ctx, device, null);
        isValidCQ(cq);
        cl.releaseCommandQueue(cq);
      });
    });

    it("should fail given an invalid property", function () {
      testUtils.withContext(function(ctx, device) {
        cl.createCommandQueue.bind(cl.createCommandQueue,ctx, device, -1).should.throw(cl.INVALID_VALUE.message);
      });
    });

    it("should fail given an invalid device", function() {
      testUtils.withContext(function(ctx, device){
        cl.createCommandQueue.bind(cl.createCommandQueue, ctx, "test", []).should.throw(cl.INVALID_DEVICE.message);
      });
    });

  });

  describe("#retainCommandQueue", function() {
    // FIXME
  });
  describe("#releaseCommandQueue", function() {
    // FIXME
  });

  describe("#getCommandQueueInfo", function() {
    // FIXME
  });

  describe("#flush", function() {

    it("should return undefined", function () {
      // TODO
    });

  });


  describe("#finish", function() {

    it("should return undefined", function () {
      // TODO
    });

  });

  describe("#enqueueReadBuffer", function() {

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


  describe("#enqueueReadBufferRect", function() {
    it("should work with read write buffers", function () {
      // TODO
    });

    it("should work with write buffers", function () {
      // TODO
    });

  });


  describe("#enqueueWriteBuffer", function() {
    it("should work with read only buffers", function () {
      // TODO
    });

    it("should work with write buffers", function () {
      // TODO
    });

  });


  describe("#enqueueWriteBufferRect", function() {
    it("should work with read write buffers", function () {
      // TODO
    });

    it("should work with write buffers", function () {
      // TODO
    });

  });

  describe("#enqueueFillBuffer", function() {
    // FIXME
  });

  describe("#enqueueCopyBuffer", function() {
    it("should work with read only buffers", function () {
      // TODO
    });

    it("should work with write buffers", function () {
      // TODO
    });

  });


  describe("#enqueueCopyBufferRect", function() {
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


  describe("#enqueueReadImage", function() {
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

  describe("#enqueueWriteImage", function() {
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


  describe("#enqueueFillImage", function() {
    // FIXME
  });


  describe("#enqueueCopyImage", function() {
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


  describe("#enqueueCopyImageToBuffer", function() {
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

  describe("#enqueueCopyBufferToImage", function() {
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

  describe("#enqueueMapBuffer", function() {
    // FIXME
  });

  describe("#enqueueMapImage", function() {
    // FIXME
  });

  describe("#enqueueMapBuffer", function() {
    // FIXME
  });

  describe("#enqueueUnmapMemObject", function() {
    // FIXME
  });

  describe("#enqueueMigrateMemObjects", function() {
    // FIXME
  });

  describe("#enqueueNDRangeKernel", function() {

    it("should work with a valid call", function () {
      // TODO
    });

    it("should work for kernels with __attribute", function () {
      // TODO
    });

  });

  describe("#enqueueTask", function() {
    // TODO
  });

  describe("#enqueueNativeKernel", function() {
    // TODO
  });

  describe("#enqueueMarkerWithWaitList", function() {
    // TODO
  });

  describe("#enqueueBarrierWithWaitList", function() {
    // TODO
  });

});
