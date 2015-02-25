var cl = require('../lib/opencl');
var should = require('chai').should();
var assert = require("chai").assert;
var util = require('util');
var log = console.log;
var U = require("./utils/utils");
var versions = require("./utils/versions");
var Diag = require("./utils/diagnostic");

var isValidCQ = function (cq) {
  assert.isNotNull(cq);
  assert.isDefined(cq);
};

var makeCommandQueue = function (ctx, device) {
  if (U.checkVersion("1.x")) {
    return cl.createCommandQueue(ctx, device, null);
  } else {
    return cl.createCommandQueueWithProperties(ctx, device, []);
  }
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


  describe("#retainCommandQueue", function() {

    it("should have incremented ref count after call", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function(cq) {
          var before = cl.getCommandQueueInfo(cq, cl.QUEUE_REFERENCE_COUNT);
          cl.retainCommandQueue(cq);
          var after = cl.getCommandQueueInfo(cq, cl.QUEUE_REFERENCE_COUNT);
          assert(before + 1 == after);
        });
      });
    });
  });

  describe("#releaseCommandQueue", function() {

    it("should have decremented ref count after call", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function(cq) {
          var before = cl.getCommandQueueInfo(cq, cl.QUEUE_REFERENCE_COUNT);
          cl.retainCommandQueue(cq);
          cl.releaseCommandQueue(cq);
          var after = cl.getCommandQueueInfo(cq, cl.QUEUE_REFERENCE_COUNT);
          assert(before == after);
        });
      });
    });

  });

  describe("#getCommandQueueInfo", function() {
    var testForType = function (clKey, _assert) {
      it("should return the good type for " + clKey, function () {
        U.withContext(function (ctx, device) {
          U.withCQ(ctx, device, function (cq) {
            var val = cl.getCommandQueueInfo(cq, cl[clKey]);
            _assert(val);
            console.log(clKey + " = " + val);
          });
        });
      })
    };

    testForType("QUEUE_REFERENCE_COUNT", assert.isNumber.bind(assert));
    testForType("QUEUE_CONTEXT", assert.isObject.bind(assert));
    testForType("QUEUE_DEVICE", assert.isObject.bind(assert));
    testForType("QUEUE_PROPERTIES", assert.isNumber.bind(assert));
  });

  describe("#flush", function() {

    it("should return success", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          assert(cl.flush(cq) == cl.SUCCESS);
        });
      });
    });

  });


  describe("#finish", function() {

    it("should return success", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          assert(cl.finish(cq) == cl.SUCCESS);
        });
      });
    });

  });


  describe("#enqueueReadBuffer", function() {

    it("should work with valid buffers", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 8, null);
          var nbuffer = new Buffer(8);
          var ret = cl.enqueueReadBuffer(cq, buffer, true, 0, 8, nbuffer);

          assert(ret == cl.SUCCESS);
        });
      });
    });

    it("should fail if buffer is null", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var nbuffer = new Buffer(8);
          U.bind(cl.enqueueReadBuffer, cq, null, true, 0, 8, nbuffer)
            .should.throw(cl.INVALID_MEM_OBJECT.message);
        });
      });
    });


    it("should fail if output buffer is null", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_WRITE_ONLY, 8, null);
          U.bind(cl.enqueueReadBuffer, cq, buffer, true, 0, 8, null)
            .should.throw(cl.INVALID_VALUE.message);
        });
      });
    });


    it("should fail if we try to read out of bound", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 8, null);
          var nbuffer = new Buffer(8);

          U.bind(cl.enqueueReadBuffer,cq, buffer, true, 16, 1, nbuffer)
            .should.throw(cl.INVALID_VALUE.message);
        });
      });
    });

  });


  describe("#enqueueReadBufferRect", function() {

    it("should work with valid buffers", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 200, null);
          var nbuffer = new Buffer(200);
          var ret = cl.enqueueReadBufferRect(cq, buffer, true,
            [0, 0, 0], [0, 0, 0], [1, 1, 1],
            2 * 4, 0, 8 * 4, 0, nbuffer);

          assert(ret == cl.SUCCESS);
        });
      });
    });

    it("should fail if buffer is null", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var nbuffer = new Buffer(8);
          U.bind(cl.enqueueReadBuffer, cq, null, true,
            [0, 0, 0], [0, 0, 0], [4, 4, 1],
            2 * 4, 0, 8 * 4, 0, nbuffer)
            .should.throw(cl.INVALID_MEM_OBJECT.message);
        });
      });
    });


    it("should fail if output buffer is null", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_WRITE_ONLY, 8, null);
          U.bind(cl.enqueueReadBufferRect, cq, buffer, true,
            [0, 0, 0], [0, 0, 0], [4, 4, 1],
            2 * 4, 0, 8 * 4, 0, null)
            .should.throw(cl.INVALID_VALUE.message);
        });
      });
    });

  });


  describe("#enqueueWriteBuffer", function() {

    it("should work with valid buffers", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 8, null);
          var nbuffer = new Buffer(8);
          var ret = cl.enqueueWriteBuffer(cq, buffer, true, 0, 8, nbuffer);

          assert(ret == cl.SUCCESS);
        });
      });
    });

    it("should fail if buffer is null", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var nbuffer = new Buffer(8);
          U.bind(cl.enqueueWriteBuffer, cq, null, true, 0, 8, nbuffer)
            .should.throw(cl.INVALID_MEM_OBJECT.message);
        });
      });
    });


    it("should fail if output buffer is null", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_WRITE_ONLY, 8, null);
          U.bind(cl.enqueueWriteBuffer, cq, buffer, true, 0, 8, null)
            .should.throw(cl.INVALID_VALUE.message);
        });
      });
    });

  });


  describe("#enqueueWriteBufferRect", function() {

    it("should work with valid buffers", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 200, null);
          var nbuffer = new Buffer(200);
          var ret = cl.enqueueWriteBufferRect(cq, buffer, true,
            [0, 0, 0], [0, 0, 0], [1, 1, 1],
            2 * 4, 0, 8 * 4, 0, nbuffer);

          assert(ret == cl.SUCCESS);
        });
      });
    });

    it("should fail if buffer is null", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var nbuffer = new Buffer(8);
          U.bind(cl.enqueueWriteBufferRect, cq, null, true,
            [0, 0, 0], [0, 0, 0], [4, 4, 1],
            2 * 4, 0, 8 * 4, 0, nbuffer)
            .should.throw(cl.INVALID_MEM_OBJECT.message);
        });
      });
    });


    it("should fail if output buffer is null", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_WRITE_ONLY, 8, null);
          U.bind(cl.enqueueWriteBufferRect, cq, buffer, true,
            [0, 0, 0], [0, 0, 0], [4, 4, 1],
            2 * 4, 0, 8 * 4, 0, null)
            .should.throw(cl.INVALID_VALUE.message);
        });
      });
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
