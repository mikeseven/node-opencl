var cl = require('../lib/opencl');
var should = require('chai').should();
var assert = require("chai").assert;
var util = require('util');
var log = console.log;
var U = require("./utils/utils");
var versions = require("./utils/versions");
var skip = require("./utils/diagnostic");
var fs = require("fs");

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
        // could be INVALID_VALUE or CL_INVALID_QUEUE_PROPERTIES etc...
        U.bind(f,ctx, device, -1).should.throw();
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
          cl.releaseCommandQueue(cq);
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
          cl.releaseMemObject(buffer);
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
          cl.releaseMemObject(buffer);
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
          cl.releaseMemObject(buffer);
        });
      });
    });

    it("should return an event", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 8, null);
          var nbuffer = new Buffer(8);
          var ret = cl.enqueueReadBuffer(cq, buffer, true, 0, 8, nbuffer, null, true);
          assert.isObject(ret);
          cl.releaseEvent(ret);
          cl.releaseMemObject(buffer);
        });
      });
    });

    it("should fire the event", function (done) {
      U.withAsyncContext(function (ctx, device, platform, ctxDone) {
        var cq = makeCommandQueue(ctx, device);

        var buffer = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 8, null);
        var nbuffer = new Buffer(8);
        var ret = cl.enqueueReadBuffer(cq, buffer, true, 0, 8, nbuffer, null, true);

        cl.setEventCallback(ret, cl.COMPLETE, function(){
          cl.releaseEvent(ret);
          cl.releaseMemObject(buffer);
          ctxDone();
          done();
        }, {});

      });
    })

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

          cl.releaseMemObject(buffer);
  
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

          cl.releaseMemObject(buffer);
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

          cl.releaseMemObject(buffer);

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

          cl.releaseMemObject(buffer);
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

          cl.releaseMemObject(buffer);

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

          cl.releaseMemObject(buffer);
        });
      });
    });

  });

  versions(["1.2", "2.0"]).describe("#enqueueFillBuffer", function() {
    it("should fill a buffer with a scallar integer pattern", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function(cq) {
          var array = new Buffer([0,0,0,0,0,0,0,0]);

          // INTEGER SCALAR
          var pattern = 2;

          var buffer = cl.createBuffer(ctx, cl.MEM_USE_HOST_PTR, 32, array);
          var ret = cl.enqueueFillBuffer(cq, buffer, 2, 0, 16);

          cl.releaseMemObject(buffer);
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should fill a buffer with a scallar float pattern", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function(cq) {
          var array = new Buffer([0,0,0,0,0,0,0,0]);

          // FLOAT SCALAR
          var pattern = 2.5;

          var buffer = cl.createBuffer(ctx, cl.MEM_USE_HOST_PTR, 32, array);
          var ret = cl.enqueueFillBuffer(cq, buffer, 2.5, 0, 16);
          cl.releaseMemObject(buffer);
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should fill a buffer with a vector pattern", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function(cq) {
          var array = new Buffer([0,0,0,0,0,0,0,0]);

          // INTEGER VECTOR
          var pattern = new Buffer([1,2]);

          var buffer = cl.createBuffer(ctx, cl.MEM_USE_HOST_PTR, 32, array);
          var ret = cl.enqueueFillBuffer(cq, buffer, pattern, 0, 16);
          cl.releaseMemObject(buffer);
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should fill a buffer with a vector pattern", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function(cq) {
          var array = new Buffer([0,0,0,0,0,0,0,0]);

          // FLOAT VECTOR
          var pattern = new Buffer([1.5,2.5]);

          var buffer = cl.createBuffer(ctx, cl.MEM_USE_HOST_PTR, 32, array);
          var ret = cl.enqueueFillBuffer(cq, buffer, pattern, 0, 16);
          cl.releaseMemObject(buffer);
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });
  });

  describe("#enqueueCopyBuffer", function() {
    it("should work with read only buffers", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_COPY_HOST_PTR, 32, new Buffer(32));
          var dst = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 8, null);
          var ret = cl.enqueueCopyBuffer(cq, buffer, dst, 0, 0, 8);

          cl.releaseMemObject(buffer);

          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should work with write buffers", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_COPY_HOST_PTR, 32, new Buffer(32));
          var dst = cl.createBuffer(ctx, cl.MEM_WRITE_ONLY, 8, null);
          var ret = cl.enqueueCopyBuffer(cq, buffer, dst, 0, 0, 8);

          cl.releaseMemObject(buffer);

          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

  });


  describe("#enqueueCopyBufferRect", function() {
    it("should work with read only buffers", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_COPY_HOST_PTR, 32, new Buffer(32));
          var dst = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 32, null);
          var ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [0, 0, 0], [0, 0, 0], [4, 4, 1],
            0, 0,
            0, 0);

          cl.releaseMemObject(buffer);
  
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should work with write buffers", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_COPY_HOST_PTR, 32, new Buffer(32));
          var dst = cl.createBuffer(ctx, cl.MEM_WRITE_BUFFER, 32, null);
          var ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [0, 0, 0], [0, 0, 0], [4, 4, 1],
            0, 0,
            0, 0);


          cl.releaseMemObject(buffer);
  
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    skip().vendor('nVidia').it("should throw cl.MEM_COPY_OVERLAP on overlapping copies", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_COPY_HOST_PTR, 64, new Buffer(64));
          U.bind(cl.enqueueCopyBufferRect, cq, buffer, buffer,
            [1, 1, 0], [2, 2, 0], [4, 4, 1],
            0, 0,
            0, 0)
          .should.throw(cl.MEM_COPY_OVERLAP.message);

          cl.releaseMemObject(buffer);
        });
      });
    });

    it("should work with different buffer origin values", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_COPY_HOST_PTR, 64, new Buffer(64));
          var dst = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 64, null);
          var ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [1, 1, 1], [2, 2, 2], [4, 4, 1],
            0, 0,
            0, 0);

          assert.strictEqual(ret, cl.SUCCESS);

          ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [1, 2, 0], [2, 2, 2], [4, 4, 1],
            0, 0,
            0, 0);

          cl.releaseMemObject(buffer);
  
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should work with different host origin values", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_COPY_HOST_PTR, 64, new Buffer(64));
          var dst = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 64, null);
          var ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [0, 0, 0], [1, 1, 0], [4, 4, 1],
            0, 0,
            0, 0);

          assert.strictEqual(ret, cl.SUCCESS);

          ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [0, 0, 0], [2, 2, 0], [4, 4, 1],
            0, 0,
            0, 0);

          assert.strictEqual(ret, cl.SUCCESS);

          ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [0, 0, 0], [1, 2, 1], [4, 4, 1],
            0, 0,
            0, 0);

          cl.releaseMemObject(buffer);
  
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should work with different region values", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_COPY_HOST_PTR, 64, new Buffer(64));
          var dst = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 64, null);
          var ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [0, 0, 0], [0, 0, 0], [1, 1, 1],
            0, 0,
            0, 0);

          assert.strictEqual(ret, cl.SUCCESS);

          ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [0, 0, 0], [0, 0, 0], [1, 4, 1],
            0, 0,
            0, 0);

          assert.strictEqual(ret, cl.SUCCESS);

          ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [0, 0, 0], [0, 0, 0], [5, 1, 1],
            0, 0,
            0, 0);

          cl.releaseMemObject(buffer);
  
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });

    });

    it("should work with different row pitch values", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_COPY_HOST_PTR, 64, new Buffer(64));
          var dst = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 64, null);
          var ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [0, 0, 0], [0, 0, 0], [1, 1, 1],
            1, 0,
            0, 0);

          assert.strictEqual(ret, cl.SUCCESS);

          ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [0, 0, 0], [0, 0, 0], [2, 4, 1],
            2, 0,
            0, 0);

          cl.releaseMemObject(buffer);
  
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should work with different splice pitch values", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer = cl.createBuffer(ctx, cl.MEM_COPY_HOST_PTR, 64, new Buffer(64));
          var dst = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 64, null);
          var ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [0, 0, 0], [0, 0, 0], [1, 1, 1],
            1, 2,
            0, 0);

          assert.strictEqual(ret, cl.SUCCESS);

          ret = cl.enqueueCopyBufferRect(cq, buffer, dst,
            [0, 0, 0], [0, 0, 0], [2, 4, 1],
            2, 2*4,
            0, 0);

          cl.releaseMemObject(buffer);
  
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should work with different host pointer values", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buffer1 = cl.createBuffer(ctx, cl.MEM_COPY_HOST_PTR, 64, new Buffer(64));
          var dst = cl.createBuffer(ctx, cl.MEM_READ_ONLY, 64, null);
          var ret = cl.enqueueCopyBufferRect(cq, buffer1, dst,
            [0, 0, 0], [0, 0, 0], [2, 4, 1],
            0, 0,
            0, 0);

          assert.strictEqual(ret, cl.SUCCESS);


          var buffer2 = cl.createBuffer(ctx, cl.MEM_USE_HOST_PTR, 64, new Buffer(64));
          ret = cl.enqueueCopyBufferRect(cq, buffer2, dst,
            [0, 0, 0], [0, 0, 0], [2, 4, 1],
            0, 0,
            0, 0);

          cl.releaseMemObject(buffer1);
          cl.releaseMemObject(dst);
  
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });
  });

  var createImageWrapper = function (ctx,flags, imageFormat, imageDesc, hostmem) {
    if(!cl.v12) { // ~=cl.v11 && !cl.v12
      return cl.createImage2D(ctx, flags, imageFormat, imageDesc.width, imageDesc.height, 0, hostmem);
    }
    else {
      return cl.createImage(ctx, flags, imageFormat, imageDesc, hostmem);
    }
  };

  describe.skip("#enqueueReadImage", function() {

    var imageFormat = {"channel_order": cl.RGBA, "channel_data_type": cl.UNSIGNED_INT8};
    var imageDesc = {
      "type": cl.MEM_OBJECT_IMAGE2D,
      "width": 8,
      "height": 8,
      "depth": 2,
      "image_array_size": 1,
      "image_row_pitch": 8,
      "image_slice_pitch": 64
    };

    it("should work with valid image", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, 0, imageFormat, imageDesc);
          // command queue, cl_image, blocking, origin, region, row, slice, ptr
          var ret = cl.enqueueReadImage(cq, image, true, [0,0,0], [8,8,1], 0, 0, new Buffer(32));
          cl.releaseMemObject(image);
          assert(ret == cl.SUCCESS);
        });
      });
    });

    skip().device("AMD").os("darwin").it("should fail with bad parameters", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          U.bind(cl.enqueueReadImage, cq, null, true, [0,0,0], [8,8,1], 0, 0, new Buffer(64))
            .should.throw(cl.INVALID_MEM_OBJECT.message);
        });
      });
    });

    skip().device("AMD").os("darwin").it("should throw cl.INVALID_OPERATION if image was created with cl.MEM_HOST_WRITE_ONLY", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_HOST_WRITE_ONLY, imageFormat, imageDesc);
          // this will cause an INVALID_VALUE exception
          var invalidRowPitch = 1000;
          U.bind(cl.enqueueReadImage, cq, image, true, [0,0,0], [8,8,1], 12, 1000, new Buffer(64))
            .should.throw(cl.INVALID_OPERATION.message);
          cl.releaseMemObject(image);
        });
      });
    });

    skip().device("AMD").os("darwin").it("should work if image was created with cl.MEM_HOST_READ_ONLY", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_HOST_READ_ONLY, imageFormat, imageDesc);
          // this will cause an INVALID_VALUE exception
          var invalidRowPitch = 1000;
          var ret = cl.enqueueReadImage(cq, image, true, [0,0,0], [8,8,1], 12, 1000, new Buffer(64));
          cl.releaseMemObject(image);

          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    skip().device("AMD").device("Intel").os("darwin").it("should throw cl.INVALID_VALUE if origin has an invalid value", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, 0, imageFormat, imageDesc);
          // this will cause an INVALID_VALUE exception
          var invalidOrigin = [1, 1, 1];
          U.bind(cl.enqueueReadImage, cq, image, true, invalidOrigin, [8,8,1], 0, 0, new Buffer(64))
            .should.throw(cl.INVALID_VALUE.message);
          cl.releaseMemObject(image);
        });
      });
    });

    skip().device("AMD").device("Intel").os("darwin").it("should throw cl.INVALID_VALUE if region is out of bound", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, 0, imageFormat, imageDesc);
          // this will cause an INVALID_VALUE exception
          var outOfBoundRegion = [9,9,1];
          U.bind(cl.enqueueReadImage, cq, image, true, [0,0,0], outOfBoundRegion, 0, 0, new Buffer(64))
            .should.throw(cl.INVALID_VALUE.message);
          cl.releaseMemObject(image);
        });
      });
    });

    skip().device("AMD").os("darwin").it("should throw cl.INVALID_VALUE if region is invalid", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, 0, imageFormat, imageDesc);
          // This will cause an INVALID_VALUE exception
          // (region[2] must be 1 for 2D images)
          var invalidRegion = [8,8,2];
          U.bind(cl.enqueueReadImage, cq, image, true, [0,0,0], invalidRegion, 0, 0, new Buffer(64))
            .should.throw(cl.INVALID_VALUE.message);
          cl.releaseMemObject(image);
        });
      });
    });

  });

  describe("#enqueueWriteImage", function() {

    var imageFormat = {"channel_order": cl.RGBA, "channel_data_type": cl.UNSIGNED_INT8};
    var imageDesc = {
      "type": cl.MEM_OBJECT_IMAGE2D,
      "width": 8,
      "height": 8,
      "depth": 2,
      "image_array_size": 1,
      "image_row_pitch": 8,
      "image_slice_pitch": 64
    };

    it("should work with cl.MEM_READ_WRITE images", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, 0, imageFormat, imageDesc);
          // command queue, cl_image, blocking, origin, region, row, slice, ptr
          var ret = cl.enqueueWriteImage(cq, image, true, [0,0,0], [8,8,1], 0, 0, new Buffer(32));
          cl.releaseMemObject(image);
          assert(ret == cl.SUCCESS);
        });
      });
    });

    skip().device("AMD").os("darwin").it("should work with cl.MEM_HOST_WRITE_ONLY images", function () {
      this.skip();
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_HOST_WRITE_ONLY, imageFormat, imageDesc, null);
          // command queue, cl_image, blocking, origin, region, row, slice, ptr
          var ret = cl.enqueueWriteImage(cq, image, true, [0,0,0], [8,8,1], 0, 0, new Buffer(32));
          cl.releaseMemObject(image);
          assert(ret == cl.SUCCESS);
        });
      });
    });

    skip().device("AMD").os("darwin").it("should throw cl.INVALID_OPERATION with cl.MEM_HOST_READ_ONLY images", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_HOST_READ_ONLY, imageFormat, imageDesc, null);

          U.bind(cl.enqueueWriteImage, cq, image, true, [0,0,0], [8,8,1], 0, 0, new Buffer(32))
            .should.throw(cl.INVALID_OPERATION.message)
          cl.releaseMemObject(image);
        });
      });
    });

    skip().device("AMD").os("darwin").it("should throw cl.INVALID_VALUE with an invalid origin", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, 0, imageFormat, imageDesc);

          // This will trigger a cl.INVALID_VALUE exception
          // (origin must be [0,0,0]
          var invalidOrigin = [1,1,1];
          U.bind(cl.enqueueWriteImage, cq, image, true, invalidOrigin, [8,8,1], 0, 0, new Buffer(32))
            .should.throw(cl.INVALID_VALUE.message)
          cl.releaseMemObject(image);
        });
      });
    });

    skip().device("AMD").os("darwin").it("should throw cl.INVALID_VALUE with an invalid region", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, 0, imageFormat, imageDesc);

          // This will trigger a cl.INVALID_VALUE exception
          // (region[2] must be 1 for 2D images)
          var invalidRegion = [8,8,2];
          U.bind(cl.enqueueWriteImage, cq, image, true, [0, 0, 0], invalidRegion, 0, 0, new Buffer(32))
            .should.throw(cl.INVALID_VALUE.message)
          cl.releaseMemObject(image);
        });
      });
    });

    skip().device("AMD").os("darwin").it("should throw cl.INVALID_VALUE if region is out of bound", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, 0, imageFormat, imageDesc);

          // This will trigger a cl.INVALID_VALUE exception
          // (region[2] must be 1 for 2D images)
          var outOfBoundRegion = [9,9,1];
          U.bind(cl.enqueueWriteImage, cq, image, true, [0, 0, 0], outOfBoundRegion, 0, 0, new Buffer(32))
            .should.throw(cl.INVALID_VALUE.message)
          cl.releaseMemObject(image);
        });
      });
    });
  });


  versions(["1.2", "2.0"]).describe("#enqueueFillImage", function() {

    var imageFormat = {"channel_order": cl.RGBA, "channel_data_type": cl.UNSIGNED_INT8};
    var imageDesc = {
      "type": cl.MEM_OBJECT_IMAGE2D,
      "width": 8,
      "height": 8,
      "depth": 2,
      "image_array_size": 1,
      "image_row_pitch": 8,
      "image_slice_pitch": 64
    };

    it("should fill image with color", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, 0, imageFormat, imageDesc);
          var color = new Buffer([0.5,0.5,0.5,0.5]);

          var ret = cl.enqueueFillImage(cq, image, color, [0,0,0], [8,8,1]);

          cl.releaseMemObject(image);
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should throw cl.INVALID_VALUE if color is null", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, 0, imageFormat, imageDesc);
          var color = null

          U.bind(cl.enqueueFillImage, cq, image, color, [0,0,0], [8,8,1])
            .should.throw(cl.INVALID_VALUE.message);
          cl.releaseMemObject(image);
        });
      });
    });

    it("should throw cl.INVALID_VALUE if region is out of bounds", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, 0, imageFormat, imageDesc);
          var color = new Buffer([0.5, 0.5, 0.5, 0.5]);
          var outOfBoundsRegion = [9,9,1];

          U.bind(cl.enqueueFillImage, cq, image, color, [0,0,0], outOfBoundsRegion)
            .should.throw(cl.INVALID_VALUE.message);
          cl.releaseMemObject(image);
        });
      });
    });

    it("should throw cl.INVALID_VALUE if origin is invalid", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, 0, imageFormat, imageDesc);
          var color = new Buffer([0.5, 0.5, 0.5, 0.5]);

          // origin[2] must be 0
          var invalidOrigin = [0,0,1];

          U.bind(cl.enqueueFillImage, cq, image, color, invalidOrigin, [8,8,1])
            .should.throw(cl.INVALID_VALUE.message);
          cl.releaseMemObject(image);
        });
      });
    });

    it("should throw cl.INVALID_VALUE if region is invalid", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, 0, imageFormat, imageDesc);
          var color = new Buffer([0.5, 0.5, 0.5, 0.5]);

          // origin[2] must be 1
          var invalidRegion = [8,8,0];

          U.bind(cl.enqueueFillImage, cq, image, color, [0,0,0], invalidRegion)
            .should.throw(cl.INVALID_VALUE.message);
          cl.releaseMemObject(image);
        });
      });
    });

    it("should throw cl.INVALID_MEM_OBJECT if image is not a valid image object", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = null;
          var color = new Buffer([0.5, 0.5, 0.5, 0.5]);

          // origin[2] must be 1
          var invalidRegion = [8,8,0];

          U.bind(cl.enqueueFillImage, cq, image, color, [0,0,0], invalidRegion)
            .should.throw(cl.INVALID_MEM_OBJECT.message);
        });
      });
    });
  });

  describe("#enqueueCopyImage", function() {

    var imageFormat = {"channel_order": cl.RGBA, "channel_data_type": cl.UNSIGNED_INT8};
    var imageDesc = {
      "type": cl.MEM_OBJECT_IMAGE2D,
      "width": 8,
      "height": 8,
      "depth": 2,
      "image_array_size": 1,
      "image_row_pitch": 8,
      "image_slice_pitch": 64
    };

    it("should work with cl.MEM_READ_WRITE images", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image1 = createImageWrapper(ctx, cl.MEM_READ_WRITE, imageFormat, imageDesc, null);
          var image2 = createImageWrapper(ctx, cl.MEM_READ_WRITE, imageFormat, imageDesc, null);
          // command queue, cl_image_src, cl_image_dst, origin_src, origin_dst, region
          var ret = cl.enqueueCopyImage(cq, image1, image2, [0, 0, 0], [0, 0, 0], [8, 8, 1]);

          cl.releaseMemObject(image1);
          cl.releaseMemObject(image2);
          assert(ret == cl.SUCCESS);
        });
      });
    });

    it("should work with write images", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image1 = createImageWrapper(ctx, cl.MEM_HOST_WRITE_ONLY, imageFormat, imageDesc, null);
          var image2 = createImageWrapper(ctx, cl.MEM_HOST_WRITE_ONLY, imageFormat, imageDesc, null);
          // command queue, cl_image_src, cl_image_dst, origin_src, origin_dst, region
          var ret = cl.enqueueCopyImage(cq, image1, image2, [0, 0, 0], [0, 0, 0], [8, 8, 1]);

          cl.releaseMemObject(image1);
          cl.releaseMemObject(image2);
          assert(ret == cl.SUCCESS);
        });
      });
    });

    it("should work with read images", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image1 = createImageWrapper(ctx, cl.MEM_HOST_READ_ONLY, imageFormat, imageDesc, null);
          var image2 = createImageWrapper(ctx, cl.MEM_HOST_READ_ONLY, imageFormat, imageDesc, null);
          // command queue, cl_image_src, cl_image_dst, origin_src, origin_dst, region
          var ret = cl.enqueueCopyImage(cq, image1, image2, [0, 0, 0], [0, 0, 0], [8, 8, 1]);

          cl.releaseMemObject(image1);
          cl.releaseMemObject(image2);
          assert(ret == cl.SUCCESS);
        });
      });
    });
  });


  describe("#enqueueCopyImageToBuffer", function() {

    var imageFormat = {"channel_order": cl.RGBA, "channel_data_type": cl.UNSIGNED_INT8};
    var imageDesc = {
      "type": cl.MEM_OBJECT_IMAGE2D,
      "width": 8,
      "height": 8,
      "depth": 2,
      "image_array_size": 1,
      "image_row_pitch": 8,
      "image_slice_pitch": 64
    };

    it("should work with read only buffers", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_COPY_HOST_PTR, imageFormat, imageDesc, new Buffer(64));
          var buffer = cl.createBuffer(ctx, cl.MEM_HOST_READ_ONLY, 64, null);

          var ret = cl.enqueueCopyImageToBuffer(cq, image, buffer, [0, 0, 0], [1, 1, 1], 0);

          cl.releaseMemObject(image);
          cl.releaseMemObject(buffer);
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should work with write buffers", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_COPY_HOST_PTR, imageFormat, imageDesc, new Buffer(64));
          var buffer = cl.createBuffer(ctx, cl.MEM_HOST_WRITE_ONLY, 64, null);

          var ret = cl.enqueueCopyImageToBuffer(cq, image, buffer, [0, 0, 0], [1, 1, 1], 0);

          cl.releaseMemObject(image);
          cl.releaseMemObject(buffer);
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should work with different values of source and destination offsets", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_COPY_HOST_PTR, imageFormat, imageDesc, new Buffer(64));
          var buffer = cl.createBuffer(ctx, cl.MEM_HOST_READ_ONLY, 64, null);

          var ret = cl.enqueueCopyImageToBuffer(cq, image, buffer, [1, 1, 0], [1, 1, 1], 2);

          cl.releaseMemObject(image);
          cl.releaseMemObject(buffer);
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should throw cl.INVALID_VALUE if origin is invalid", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_COPY_HOST_PTR, imageFormat, imageDesc, new Buffer(64));
          var buffer = cl.createBuffer(ctx, cl.MEM_HOST_READ_ONLY, 64, null);

          // origin[2] must be 0
          var invalidOrigin = [1,1,1];
          U.bind(cl.enqueueCopyImageToBuffer, cq, image, buffer, invalidOrigin, [1, 1, 1], 2)
            .should.throw(cl.INVALID_VALUE.message);

          cl.releaseMemObject(image);
          cl.releaseMemObject(buffer);
        });
      });
    });

    it("should throw cl.INVALID_VALUE if region is invalid", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_COPY_HOST_PTR, imageFormat, imageDesc, new Buffer(64));
          var buffer = cl.createBuffer(ctx, cl.MEM_HOST_READ_ONLY, 64, null);

          // region[2] must be 1
          var invalidRegion = [1,1,2];
          U.bind(cl.enqueueCopyImageToBuffer, cq, image, buffer, [1, 1, 0], invalidRegion, 2)
            .should.throw(cl.INVALID_VALUE.message);

          cl.releaseMemObject(image);
          cl.releaseMemObject(buffer);
        });
      });
    });
  });

  describe("#enqueueCopyBufferToImage", function() {

    var imageFormat = {"channel_order": cl.RGBA, "channel_data_type": cl.UNSIGNED_INT8};
    var imageDesc = {
      "type": cl.MEM_OBJECT_IMAGE2D,
      "width": 8,
      "height": 8,
      "depth": 2,
      "image_array_size": 1,
      "image_row_pitch": 8,
      "image_slice_pitch": 64
    };

    it("should work with read only buffers", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_HOST_READ_ONLY, imageFormat, imageDesc, null);
          var buffer = cl.createBuffer(ctx, cl.MEM_HOST_READ_ONLY, 8, null);

          var ret = cl.enqueueCopyBufferToImage(cq, buffer, image, 0, [0, 0, 0], [1, 1, 1]);

          cl.releaseMemObject(image);
          cl.releaseMemObject(buffer);
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should work with write buffers", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_HOST_WRITE_ONLY, imageFormat, imageDesc, null);
          var buffer = cl.createBuffer(ctx, cl.MEM_HOST_WRITE_ONLY, 8, null);

          var ret = cl.enqueueCopyBufferToImage(cq, buffer, image, 0, [0, 0, 0], [1, 1, 1]);

          cl.releaseMemObject(image);
          cl.releaseMemObject(buffer);
          assert.strictEqual(ret, cl.SUCCESS);
        });
      });
    });

    it("should throw cl.INVALID_VALUE if origin is invalid", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_HOST_WRITE_ONLY, imageFormat, imageDesc, null);
          var buffer = cl.createBuffer(ctx, cl.MEM_HOST_WRITE_ONLY, 8, null);

          // origin[2] must be 0
          var invalidOrigin = [1,1,1];
          U.bind(cl.enqueueCopyBufferToImage, cq, buffer, image, 0, invalidOrigin, [1, 1, 1])
            .should.throw(cl.INVALID_VALUE.message);

          cl.releaseMemObject(image);
          cl.releaseMemObject(buffer);
        });
      });
    });

    it("should throw cl.INVALID_VALUE if region is invalid", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_HOST_WRITE_ONLY, imageFormat, imageDesc, null);
          var buffer = cl.createBuffer(ctx, cl.MEM_HOST_WRITE_ONLY, 8, null);

          // region[2] must be 1
          var invalidRegion = [1,1,2];
          U.bind(cl.enqueueCopyBufferToImage, cq, buffer, image, 0, [1, 1, 0], invalidRegion)
            .should.throw(cl.INVALID_VALUE.message);

          cl.releaseMemObject(image);
          cl.releaseMemObject(buffer);
        });
      });
    });
  });

  describe("# enqueueMapBuffer", function() {
    it("should return a valid buffer", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.createBuffer(ctx, cl.MEM_READ_WRITE, 8, null);
          var ret = cl.enqueueMapBuffer(cq, buf, true, cl.MAP_READ, 0, 8,[],false);
          var u8s = new Uint8Array(ret);
          assert.instanceOf(u8s.buffer, ArrayBuffer);
          assert.equal(u8s.buffer.byteLength, 8);
          assert.isNumber(u8s[0]);
        });
      });
    });


    it("should not be able to read from a not already allocated pointer", function () {
      U.withContext(function (ctx, device, _) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.createBuffer(ctx, 0, 8, null);
          var ret = cl.enqueueMapBuffer(cq, buf, false, cl.MAP_READ, 0, 8, [], true);
          var u8s = new Uint8Array(ret);
          assert.isNumber(u8s[0]);
          assert.equal(u8s.buffer.byteLength, 8);
        });
      });
    });

    it("should not throw as we are using the pointer from an event", function (done) {
      U.withAsyncContext(function (ctx, device, _, ctxDone) {
        U.withAsyncCQ(ctx, device, function (cq, cqDone) {
          var buf = cl.createBuffer(ctx, 0, 8, null);
          var ret = cl.enqueueMapBuffer(cq, buf, false, 0, 0, 8, [], true);

          cl.setEventCallback(ret.event, cl.COMPLETE, function(){
            var u8s = new Uint8Array(ret);
            assert.instanceOf(u8s.buffer, ArrayBuffer);
            assert.equal(u8s.buffer.byteLength, 8);
            assert.isNumber(u8s[0]);
            cl.releaseMemObject(buf);
            cl.releaseEvent(ret.event);
            cqDone();
            ctxDone();
            done();
          });
        });
      });
    });
  });

  describe("# enqueueMapImage", function() {

    var imageFormat = {"channel_order": cl.RGBA, "channel_data_type": cl.UNSIGNED_INT8};
    var imageDesc = {
      "type": cl.MEM_OBJECT_IMAGE2D,
      "width": 10,
      "height": 10,
      "depth": 1,
      "image_array_size": 1
    };

    it("should return a valid buffer", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = cl.createImage(ctx, 0, imageFormat, imageDesc, null);
          var ret = cl.enqueueMapImage(cq, image, true, cl.MAP_READ, [0,0,0], [2,2,1]);
          var u8s = new Uint8Array(ret);
          assert.instanceOf(u8s.buffer, ArrayBuffer);
          assert.isNumber(u8s[0]);
          cl.releaseMemObject(image);
        });
      });
    });

    it("should return a valid buffer", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = cl.createImage(ctx, 0, imageFormat, imageDesc, null);
          var ret = cl.enqueueMapImage(cq, image, true, cl.MAP_WRITE, [0,0,0], [2,2,1]);
          var u8s = new Uint8Array(ret);
          assert.instanceOf(u8s.buffer, ArrayBuffer);
          assert.isNumber(u8s[0]);
          cl.releaseMemObject(image);
        });
      });
    });

    it("should return a valid buffer", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = cl.createImage(ctx, 0, imageFormat, imageDesc, null);
          var ret = cl.enqueueMapImage(cq, image, true, cl.MAP_WRITE_INVALIDATE_REGION, [0,0,0], [2,2,1]);
          var u8s = new Uint8Array(ret);
          assert.instanceOf(u8s.buffer, ArrayBuffer);
          assert.isNumber(u8s[0]);
          cl.releaseMemObject(image);
        });
      });
    });


    it("should not be able to read from a not already allocated pointer", function () {
      U.withContext(function (ctx, device, _) {
        U.withCQ(ctx, device, function (cq) {
          var image = cl.createImage(ctx, 0, imageFormat, imageDesc, null);
          var ret = cl.enqueueMapImage(cq, image, false, cl.MAP_READ, [0,0,0], [2,2,1], [], true);
          var u8s = new Uint8Array(ret);
          assert.instanceOf(u8s.buffer, ArrayBuffer);
          assert.isNumber(u8s[0]);
          cl.releaseMemObject(image);
        });
      });
    });

    it("should not be able to read from a not already allocated pointer", function () {
      U.withContext(function (ctx, device, _) {
        U.withCQ(ctx, device, function (cq) {
          var image = cl.createImage(ctx, 0, imageFormat, imageDesc, null);
          var ret = cl.enqueueMapImage(cq, image, false, cl.MAP_WRITE, [0,0,0], [2,2,1], [], true);
          var u8s = new Uint8Array(ret);
          assert.instanceOf(u8s.buffer, ArrayBuffer);
          assert.isNumber(u8s[0]);
          cl.releaseMemObject(image);
        });
      });
    });

    it("should not throw as we are using the pointer from an event", function (done) {
      U.withAsyncContext(function (ctx, device, _, ctxDone) {
        U.withAsyncCQ(ctx, device, function (cq, cqDone) {
          var image = cl.createImage(ctx, 0, imageFormat, imageDesc, null);
          var ret = cl.enqueueMapImage(cq, image, false, cl.MAP_READ, [0,0,0], [2,2,1], [], true);

          cl.setEventCallback(ret.event, cl.COMPLETE, function(){
            var u8s = new Uint8Array(ret);
            assert.instanceOf(u8s.buffer, ArrayBuffer);
            assert.isNumber(u8s[0]);
            cl.releaseMemObject(image);
            cl.releaseEvent(ret.event);
            cqDone();
            ctxDone();
            done();
          });
        });
      });
    });

    it("should not throw as we are using the pointer from an event", function (done) {
      U.withAsyncContext(function (ctx, device, _, ctxDone) {
        U.withAsyncCQ(ctx, device, function (cq, cqDone) {
          var image = cl.createImage(ctx, 0, imageFormat, imageDesc, null);
          var ret = cl.enqueueMapImage(cq, image, false, cl.MAP_WRITE, [0,0,0], [2,2,1], [], true);

          cl.setEventCallback(ret.event, cl.COMPLETE, function(){
            var u8s = new Uint8Array(ret);
            assert.instanceOf(u8s.buffer, ArrayBuffer);
            assert.isNumber(u8s[0]);
            cl.releaseMemObject(image);
            cl.releaseEvent(ret.event);
            ctxDone();
            cqDone();
            done();
          });
        });
      });

    });

  });

  describe("# enqueueUnmapMemObject", function() {
    it("should throw as we are unmapping a non mapped memobject", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.createBuffer(ctx, 0, 8, null);
          U.bind(cl.enqueueUnmapMemObject, cq, buf, new Buffer(8)).should.throw(/*...*/);
          cl.releaseMemObject(buf);
        });
      });
    });

    it("should return success", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.createBuffer(ctx, 0, 8, null);
          var ret = cl.enqueueMapBuffer(cq, buf, true, cl.MAP_READ, 0, 8,[],false);
          var u8s = new Uint8Array(ret);
          assert.instanceOf(u8s.buffer, ArrayBuffer);
          var res = cl.enqueueUnmapMemObject(cq, buf, u8s.buffer);
          assert.equal(res, cl.SUCCESS);
          cl.releaseMemObject(buf);
        });
      });
    });
  });

  versions(["1.2","2.0"]).describe("#enqueueMigrateMemObjects", function() {
    var imageFormat = {"channel_order": cl.RGBA, "channel_data_type": cl.UNSIGNED_INT8};
    var imageDesc = {
      "type": cl.MEM_OBJECT_IMAGE2D,
      "width": 8,
      "height": 8,
      "depth": 2,
      "image_array_size": 1,
      "image_row_pitch": 8,
      "image_slice_pitch": 64
    };

    it("should migrate mem objects with flag cl.MIGRATE_MEM_OBJECT_HOST", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_COPY_HOST_PTR, imageFormat, imageDesc, new Buffer(64));
          var buffer = cl.createBuffer(ctx, cl.MEM_HOST_READ_ONLY, 64, null);

          // cq, mem objects, flags
          var ret = cl.enqueueMigrateMemObjects(cq, [image, buffer], cl.MIGRATE_MEM_OBJECT_HOST);

          assert.strictEqual(ret, cl.SUCCESS);
          cl.releaseMemObject(image);
          cl.releaseMemObject(buffer);
        });
      });
    });

    it("should migrate mem objects with flag cl.MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_COPY_HOST_PTR, imageFormat, imageDesc, new Buffer(64));
          var buffer = cl.createBuffer(ctx, cl.MEM_HOST_READ_ONLY, 64, null);

          // cq, mem objects, flags
          var ret = cl.enqueueMigrateMemObjects(cq, [image, buffer], cl.MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED);

          assert.strictEqual(ret, cl.SUCCESS);
          cl.releaseMemObject(image);
          cl.releaseMemObject(buffer);
        });
      });
    });

    it("should throw cl.INVALID_VALUE if memObjects is null", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          // cq, mem objects, flags
          U.bind(cl.enqueueMigrateMemObjects, cq, null, cl.MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED)
            .should.throw(cl.INVALID_VALUE.message);
        });
      });
    });

    it("should throw cl.INVALID_MEM_OBJECT if any memory object is null", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var image = createImageWrapper(ctx, cl.MEM_COPY_HOST_PTR, imageFormat, imageDesc, new Buffer(64));
          var buffer = null;

          U.bind(cl.enqueueMigrateMemObjects, cq, [image, buffer], cl.MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED)
            .should.throw(cl.INVALID_MEM_OBJECT.message);
          cl.releaseMemObject(image);
        });
      });
    });
  });

  describe("#enqueueNDRangeKernel", function() {

    var inputs = new Buffer(10000 * 4);
    var outputs = new Buffer(10000 * 4);

    for (var i = 0; i < 10000; ++ i) {
      inputs.writeUInt32LE(i, i * 4);
    }

    it("should throw with a valid call but with no bound args", function () {
      U.withContext(function (ctx, device) {
        U.withProgram(ctx, fs.readFileSync(__dirname  + "/kernels/square.cl").toString(),
        function (prg) {
          cl.buildProgram(prg);
          var kern = cl.createKernel(prg, "square");

          var inputsMem = cl.createBuffer(
            ctx, cl.MEM_COPY_HOST_PTR, 10000 * 4, inputs);
          var outputsMem = cl.createBuffer(
            ctx, cl.MEM_COPY_HOST_PTR, 10000 * 4, outputs);

          cl.setKernelArg(kern, 0, "uint*", inputsMem);
          cl.setKernelArg(kern, 1, "uint*", outputsMem);
          cl.setKernelArg(kern, 2, "uint", 10000);

          U.withCQ(ctx, device, function (cq) {

            cl.enqueueNDRangeKernel(
              cq, kern, 1, null, [100], null);

            cl.releaseMemObject(inputsMem);
            cl.releaseMemObject(outputsMem);
            cl.releaseKernel(kern);
          });
        })
      })
    });

    // AMD : It returns invalid value ...
    // skip().vendor("AMD").it("should fail with null global size", function () {
    //   U.withContext(function (ctx, device) {
    //     U.withProgram(ctx, fs.readFileSync(__dirname  + "/kernels/square.cl").toString(),
    //       function (prg) {
    //         cl.buildProgram(prg);
    //         var kern = cl.createKernel(prg, "square");

    //         var inputsMem = cl.createBuffer(
    //           ctx, cl.MEM_COPY_HOST_PTR, 10000 * 4, inputs);
    //         var outputsMem = cl.createBuffer(
    //           ctx, cl.MEM_COPY_HOST_PTR, 10000 * 4, outputs);

    //         cl.setKernelArg(kern, 0, "uint*", inputsMem);
    //         cl.setKernelArg(kern, 1, "uint*", outputsMem);
    //         cl.setKernelArg(kern, 2, "uint", 10000);

    //         U.withCQ(ctx, device, function (cq) {

    //           U.bind(cl.enqueueNDRangeKernel,
    //             cq, kern, 1, null, null, null)
    //             .should.throw(cl.INVALID_GLOBAL_WORK_SIZE.message);

    //         });
    //       })
    //   })
    // });

    it("should fail if kern is invalid", function () {
      U.withContext(function (ctx, device) {
        U.withProgram(ctx, fs.readFileSync(__dirname  + "/kernels/square.cl").toString(),
          function (prg) {
            cl.buildProgram(prg);
            var kern = cl.createKernel(prg, "square");

            var inputsMem = cl.createBuffer(
              ctx, cl.MEM_COPY_HOST_PTR, 10000 * 4, inputs);
            var outputsMem = cl.createBuffer(
              ctx, cl.MEM_COPY_HOST_PTR, 10000 * 4, outputs);

            cl.setKernelArg(kern, 0, "uint*", inputsMem);
            cl.setKernelArg(kern, 1, "uint*", outputsMem);
            cl.setKernelArg(kern, 2, "uint", 10000);

            U.withCQ(ctx, device, function (cq) {

              U.bind(cl.enqueueNDRangeKernel,
                cq, null, 1, null, [100], null)
                .should.throw(cl.INVALID_KERNEL.message);

              cl.releaseMemObject(inputsMem);
              cl.releaseMemObject(outputsMem);
              cl.releaseKernel(kern);
            });
          })
      })
    });


    it("should fail if given dimensions does not match arrays", function () {
      U.withContext(function (ctx, device) {
        U.withProgram(ctx, fs.readFileSync(__dirname  + "/kernels/square.cl").toString(),
          function (prg) {
            cl.buildProgram(prg);
            var kern = cl.createKernel(prg, "square");

            var inputsMem = cl.createBuffer(
              ctx, cl.MEM_COPY_HOST_PTR, 10000 * 4, inputs);
            var outputsMem = cl.createBuffer(
              ctx, cl.MEM_COPY_HOST_PTR, 10000 * 4, outputs);

            cl.setKernelArg(kern, 0, "uint*", inputsMem);
            cl.setKernelArg(kern, 1, "uint*", outputsMem);
            cl.setKernelArg(kern, 2, "uint", 10000);
            U.withCQ(ctx, device, function (cq) {

              U.bind(cl.enqueueNDRangeKernel,
                cq, kern, 1, null, [100, 200], null)
                .should.throw(cl.INVALID_GLOBAL_WORK_SIZE.message);

              cl.releaseMemObject(inputsMem);
              cl.releaseMemObject(outputsMem);
              cl.releaseKernel(kern);
            });
          })
      })
    });

  });

  describe("#enqueueTask", function() {

    var inputs = new Buffer(10000 * 4);
    var outputs = new Buffer(10000 * 4);

    for (var i = 0; i < 10000; ++ i) {
      inputs.writeUInt32LE(i, i * 4);
    }

    versions(["2.x"]).hasUndefined(cl.enqueueTask);

    versions(["1.x"]).it("should work with a valid call", function () {
      U.withContext(function (ctx, device) {
        U.withProgram(ctx, fs.readFileSync(__dirname  + "/kernels/square_one.cl").toString(),
          function (prg) {
            cl.buildProgram(prg);

            var kern = cl.createKernel(prg, "square_one");

            cl.setKernelArg(kern, 0, "uint", 1);
            cl.setKernelArg(kern, 1, "uint", 2);

            U.withCQ(ctx, device, function (cq) {
              cl.enqueueTask(cq, kern);
              cl.releaseKernel(kern);
            });
          })
      })
    });

    versions(["1.x"]).it("should fail if kern is invalid", function () {
      U.withContext(function (ctx, device) {
        U.withProgram(ctx, fs.readFileSync(__dirname  + "/kernels/square_one.cl").toString(),
          function (prg) {

            U.withCQ(ctx, device, function (cq) {

              U.bind(cl.enqueueTask,
                cq, null)
                .should.throw(cl.INVALID_KERNEL.message);

            });
          })
      })
    });

  });

  describe("#enqueueNativeKernel", function() {

    it("should always throw since it is not supported", function () {
      U.bind(cl.enqueueNativeKernel).should.throw();
    });

  });

  versions(["1.2","2.0"]).describe("#enqueueMarkerWithWaitList", function() {
    versions(["1.1"]).hasUndefined(cl.enqueueMarkerWithWaitList);

    it("should enqueue marker with event wait list", function() {

      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {

          var array = new Buffer([0, 0, 0, 0, 0, 0, 0, 0]);

          var buffer = cl.createBuffer(ctx, cl.MEM_USE_HOST_PTR, 32, array);
          var event = cl.enqueueFillBuffer(cq, buffer, new Buffer([1, 2]), 0, 16, null, true);

          var ret = cl.enqueueMarkerWithWaitList(cq, [event], true);

          assert.isObject(ret);

          cl.setEventCallback(ret, cl.COMPLETE, function() {
            cl.releaseMemObject(buffer);
            cl.releaseEvent(ret);
            cl.releaseEvent(event);
          });
        });
      });
    });
  });

  versions(["1.2","2.0"]).describe("#enqueueBarrierWithWaitList", function() {
    versions(["1.1"]).hasUndefined(cl.enqueueBarrierWithWaitList);

    it("should enqueue barrier with event wait list", function() {

      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {

          var array = new Buffer([0, 0, 0, 0, 0, 0, 0, 0]);

          var buffer = cl.createBuffer(ctx, cl.MEM_USE_HOST_PTR, 32, array);
          var event = cl.enqueueFillBuffer(cq, buffer, new Buffer([1, 2]), 0, 16, null, true);

          var ret = cl.enqueueBarrierWithWaitList(cq, [event], true);

          assert.isObject(ret);

          cl.setEventCallback(ret, cl.COMPLETE, function() {
            cl.releaseMemObject(buffer);
            cl.releaseEvent(ret);
            cl.releaseEvent(event);
          });
        });
      });
    });

  });

  versions(["1.1"]).describe("#enqueueBarrier", function() {
    it("should enqueue barrier", function() {

      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {

          var ret = cl.enqueueBarrier(cq);

          assert.equal(cl.SUCCESS,ret);
        });
      });
    });

  });
  versions(["1.1"]).describe("#enqueueMarker", function() {
    it("should enqueue marker", function() {

      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {

          var ret = cl.enqueueMarker(cq,true);

          assert.isObject(ret);

          cl.setEventCallback(ret, cl.COMPLETE, function() {
            cl.releaseEvent(ret);
          });
        });
      });
    });
  });
});
