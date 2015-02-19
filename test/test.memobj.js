var cl = require('../lib/opencl');
var should = require('chai').should();
var chai = require('chai');
var util = require('util');
var assert = require('chai').assert;
var U = require("./utils/utils");
var log = console.log;
var Diag = require("./utils/diagnostic");

describe("MemObj", function() {

  describe("#createBuffer", function() {

    var f = cl.createBuffer;

    it("should throw cl.INVALID_CONTEXT if context is invalid", function () {
      U.withContext(function (context, device, platform) {
        f.bind(f, null, null, null, null).should.throw(cl.INVALID_CONTEXT.message);
      });
    });

    it("should throw cl.INVALID_VALUE if flags are not valid", function () {
      U.withContext(function (context, device, platform) {
        f.bind(f, context, -1, 8, null).should.throw(cl.INVALID_VALUE.message);
      });
    });

    it("should throw cl.INVALID_BUFFER_SIZE if size is 0", function () {
      U.withContext(function (context, device, platform) {
        f.bind(f, context, 0, 0, null).should.throw(cl.INVALID_BUFFER_SIZE.message);
      });
    });

    it("should use cl.MEM_READ_WRITE as default value when flags is 0", function () {
      U.withContext(function (context, device, platform) {

        var buffer = f(context, 0, 8, null);
        //var flags = cl.getMemObjectInfo(buffer, cl.MEM_FLAGS);
        cl.releaseMemObject(buffer);
        // FIXME: Can't test this like that, getMemObjectInfo will return the value passed to createBuffer
        // instead of the actual default value used
        // assert.equal(flags, cl.MEM_READ_WRITE);
      });
    });

    it("should copy memory when passed a Buffer", function () {
      U.withContext(function (context, device, platform) {

        var array = new Buffer(32);
        var buffer = f(context, cl.MEM_COPY_HOST_PTR, 8, array);
        cl.releaseMemObject(buffer);
      });
    });

    it("should copy memory when passed a TypedArray", function () {
      U.withContext(function (context, device, platform) {

        var array = new ArrayBuffer(32);
        var i32Array = new Int32Array(array);
        var buffer = f(context, cl.MEM_COPY_HOST_PTR, 8, i32Array);
        cl.releaseMemObject(buffer);
      });
    });

    it("should throw cl.INVALID_MEM_OBJECT when passed neither a Buffer nor a TypedArray", function () {
      U.withContext(function (context, device, platform) {
        f.bind(f, context, cl.MEM_COPY_HOST_PTR, 8, String("this won't do !")).should.throw(cl.INVALID_MEM_OBJECT.message);
      });
    });

    it("should throw cl.INVALID_MEM_OBJECT when passed an Array", function () {
      U.withContext(function (context, device, platform) {
        f.bind(f, context, cl.MEM_COPY_HOST_PTR, 8, [1, 2, 3, 4, 5, 6, 7, 8]).should.throw(cl.INVALID_MEM_OBJECT.message);
      });
    });

  });
  describe("#createSubBuffer", function() {

    var f = cl.createSubBuffer;

    it("should throw cl.INVALID_MEM_OBJECT if buffer is not valid", function() {
      U.withContext(function (context, device, platform) {

        f.bind(f, null, cl.MEM_READ_WRITE, cl.BUFFER_CREATE_TYPE_REGION, {"origin": 0, "size": 2})
          .should.throw(cl.INVALID_MEM_OBJECT.message);
      });
    });

    it("should throw cl.INVALID_VALUE if buffer was created with cl.MEM_WRITE_ONLY and flags specifies CL_MEM_READ_WRITE", function() {
      U.withContext(function (context, device, platform) {

        var buffer = cl.createBuffer(context, cl.MEM_WRITE_ONLY, 8, null);

        Diag.exclude(device)
          .os("win32")
          .driver("OpenCL 2.0 AMD-APP (1642.5)")
          .gpu("Intel(R) Core(TM) i7-4600U CPU @ 2.10GHz")
          .because("It creates the subbuffer, ignoring read only constraint")
          .should(function () {
            var buf = f(buffer, cl.MEM_READ_WRITE, cl.BUFFER_CREATE_TYPE_REGION, {"origin": 0, "size": 2});
            cl.releaseMemObject(buf);
          })
          .raise();

        f.bind(f, buffer, cl.MEM_READ_WRITE, cl.BUFFER_CREATE_TYPE_REGION, {"origin": 0, "size": 2})
          .should.throw(cl.INVALID_VALUE.message);
      });
    });


    it("should throw cl.INVALID_VALUE if bufferCreateType is not BUFFER_CREATE_TYPE_CREGION", function() {
      U.withContext(function (context, device, platform) {

        var buffer = cl.createBuffer(context, 0, 8, null);

        f.bind(f, buffer, 0, -1, {"origin": 0, "size": 2}).should.throw(cl.INVALID_VALUE.message);

        cl.releaseMemObject(buffer);
      });
    });

    it("should create a subBuffer", function() {
      U.withContext(function (context, device, platform) {
        var buffer = cl.createBuffer(context, 0, 8, null);
        var subBuffer = f(buffer, 0, cl.BUFFER_CREATE_TYPE_REGION, {"origin": 0, "size": 2});

        cl.releaseMemObject(subBuffer);
        cl.releaseMemObject(buffer);
      });
    })

  });
  describe("#createImage", function() {

    var f = cl.createImage;
    var imageFormat = {"channel_order": cl.RGBA, "channel_data_type": cl.UNSIGNED_INT8};
    var imageDesc = {
      "type": cl.MEM_OBJECT_IMAGE2D,
      "width": 10,
      "height": 10,
      "depth": 8,
      "image_array_size": 1
    };

    it("should create an image", function() {
      U.withContext(function (context, device, platform) {

        var image = cl.createImage(context, 0, imageFormat, imageDesc, null);
        cl.releaseMemObject(image);
      });
    });

    it("should throw cl.INVALID_CONTEXT if context is invalid", function() {
      U.withContext(function (context, device, platform) {

        f.bind(f, null, 0, imageFormat, imageDesc, null).should.throw(cl.INVALID_CONTEXT.message);
      });
    });

    it("should throw cl.INVALID_VALUE if flags is invalid", function () {
      U.withContext(function (context, device, platform) {
        f.bind(f, context, -1, imageFormat, imageDesc, null).should.throw(cl.INVALID_VALUE.message);
      });
    });

  });
  describe("#retainMemObject", function() {

    var f = cl.retainMemObject;

    it("should retain mem object", function () {
      U.withContext(function (context, device, platform) {
        var buffer = cl.createBuffer(context, 0, 8, null);
        f(buffer);
        cl.releaseMemObject(buffer);
        cl.releaseMemObject(buffer);
      });
    });

    it("should throw cl.INVALID_MEM_OBJECT if mem object is invalid", function () {
      U.withContext(function (context, device, platform) {
        f.bind(f, null).should.throw(cl.INVALID_MEM_OBJECT.message);
      });
    });

  });
  describe("#releaseMemObject", function() {

    var f = cl.releaseMemObject;

    it("should release mem object", function () {
      U.withContext(function (context, device, platform) {
        var buffer = cl.createBuffer(context, 0, 8, null);
        f(buffer);
      });
    });

    it("should throw cl.INVALID_MEM_OBJECT if mem object is invalid", function () {
      U.withContext(function (context, device, platform) {
        f.bind(f, null).should.throw(cl.INVALID_MEM_OBJECT.message);
      });
    });

  });
  describe("#getSupportedImageFormats", function() {

    var f = cl.getSupportedImageFormats;

    it("should get supported image formats", function () {
      U.withContext(function (context, device, platform) {
        var formats = f(context, cl.MEM_READ_WRITE, cl.MEM_OBJECT_IMAGE2D);
        assert.isArray(formats);
        assert.isAbove(formats.length, 0);
      });
    });

    it("should throw cl.INVALID_CONTEXT if context is invalid", function () {
      U.withContext(function (context, device, platform) {
        f.bind(f, null, 0, cl.MEM_OBJECT_IMAGE2D).should.throw(cl.INVALID_CONTEXT.message);
      });
    });

  });
  describe("#getMemObjectInfo", function() {

    var f = cl.getMemObjectInfo;

    it("should return CL_MEM_TYPE", function () {
      U.withContext(function (context, device, platform) {
        var buffer = cl.createBuffer(context, 0, 8, null);
        var ret = f(buffer, cl.MEM_TYPE);
        assert.isNumber(ret);
        cl.releaseMemObject(buffer);
      });
    });

    it("should return CL_MEM_FLAGS", function () {
      U.withContext(function (context, device, platform) {
        var buffer = cl.createBuffer(context, 0, 8, null);
        var ret = f(buffer, cl.MEM_FLAGS);
        assert.isNumber(ret);

        Diag.exclude(device)
          .os("win32")
          .driver("OpenCL 1.2 ") // Intel
          .gpu("Intel(R) HD Graphics 4400")
          .because("It returns 1 instead of 0")
          .should(function () {
            assert.strictEqual(ret, 1);
          })
          .raise();

        assert.strictEqual(ret, 0);
        cl.releaseMemObject(buffer);
      });
    });

    it("should return CL_MEM_SIZE", function () {
      U.withContext(function (context, device, platform) {
        var buffer = cl.createBuffer(context, 0, 8, null);
        var ret = f(buffer, cl.MEM_SIZE);
        assert.isNumber(ret);
        assert.strictEqual(ret, 8);
        cl.releaseMemObject(buffer);
      });
    });

    it("should return CL_MEM_OFFSET", function () {
      U.withContext(function (context, device, platform) {
        var buffer = cl.createBuffer(context, 0, 8, null);
        var ret = f(buffer, cl.MEM_OFFSET);
        assert.isNumber(ret);
        assert.strictEqual(ret, 0);
        cl.releaseMemObject(buffer);
      });
    });

    it("should return CL_MEM_MAP_COUNT", function () {
      U.withContext(function (context, device, platform) {
        var buffer = cl.createBuffer(context, 0, 8, null);
        var ret = f(buffer, cl.MEM_MAP_COUNT);
        assert.isNumber(ret);
        assert.strictEqual(ret, 0);
        cl.releaseMemObject(buffer);
      });
    });

    it("should return CL_MEM_REFERENCE_COUNT", function () {
      U.withContext(function (context, device, platform) {
        var buffer = cl.createBuffer(context, 0, 8, null);
        var ret = f(buffer, cl.MEM_REFERENCE_COUNT);
        assert.isNumber(ret);
        assert.strictEqual(ret, 1);
        cl.releaseMemObject(buffer);
      });
    });

    it("should return CL_MEM_CONTEXT", function () {
      U.withContext(function (context, device, platform) {
        var buffer = cl.createBuffer(context, 0, 8, null);
        var ret = f(buffer, cl.MEM_CONTEXT);
        assert.isObject(ret);
        cl.releaseMemObject(buffer);
      });
    });

    it("should return CL_MEM_HOST_PTR", function () {
      U.withContext(function (context, device, platform) {
        var buffer = cl.createBuffer(context, 0, 8, null);
        var ret = f(buffer, cl.MEM_ASSOCIATED_MEMOBJECT);
        assert.isObject(ret);
        cl.releaseMemObject(buffer);
      });
    });

    it("should return CL_MEM_CONTEXT", function () {
      U.withContext(function (context, device, platform) {
        var buffer = cl.createBuffer(context, 0, 8, null);
        var ret = f(buffer, cl.MEM_CONTEXT);
        assert.isObject(ret);
        cl.releaseMemObject(buffer);
      });
    });

    it("should throw cl.INVALID_MEM_OBJECT if memory object is invalid", function () {
      U.withContext(function (context, device, platform) {
        f.bind(f, null, cl.MEM_ASSOCIATED_MEMOBJECT).should.throw(cl.INVALID_MEM_OBJECT.message);
      });
    });

  });

  describe("#getImageInfo", function() {

    var f = cl.getImageInfo;
    var imageFormat = {"channel_order": cl.RGBA, "channel_data_type": cl.UNSIGNED_INT8};
    var imageDesc = {
      "type": cl.MEM_OBJECT_IMAGE2D,
      "width": 10,
      "height": 10,
      "depth": 8,
      "image_array_size": 1
    };


    it("should return CL_IMAGE_FORMAT", function () {
      U.withContext(function (context, device, platform) {
        var image = cl.createImage(context, 0, imageFormat, imageDesc, null);
        var imageInfo = f(image, cl.IMAGE_FORMAT);
        assert.isArray(imageInfo);
      });
    });

    it("should return CL_IMAGE_ELEMENT_SIZE", function () {
      U.withContext(function (context, device, platform) {
        var image = cl.createImage(context, 0, imageFormat, imageDesc, null);
        var imageInfo = f(image, cl.IMAGE_ELEMENT_SIZE);
        assert.isNumber(imageInfo);
      });

    });

    it("should return CL_IMAGE_ROW_PITCH", function () {
      U.withContext(function (context, device, platform) {
        var image = cl.createImage(context, 0, imageFormat, imageDesc, null);
        var imageInfo = f(image, cl.IMAGE_ROW_PITCH);
        assert.isNumber(imageInfo);
      });

    });

    it("should return CL_IMAGE_SLICE_PITCH", function () {
      U.withContext(function (context, device, platform) {
        var image = cl.createImage(context, 0, imageFormat, imageDesc, null);
        var imageInfo = f(image, cl.IMAGE_ROW_PITCH);
        assert.isNumber(imageInfo);
      });

    });

    it("should return CL_IMAGE_WIDTH", function () {
      U.withContext(function (context, device, platform) {
        var image = cl.createImage(context, 0, imageFormat, imageDesc, null);
        var imageInfo = f(image, cl.IMAGE_WIDTH);
        assert.isNumber(imageInfo);
        assert.strictEqual(imageInfo, 10);
      });

    });

    it("should return CL_IMAGE_HEIGHT", function () {
      U.withContext(function (context, device, platform) {
        var image = cl.createImage(context, 0, imageFormat, imageDesc, null);
        var imageInfo = f(image, cl.IMAGE_HEIGHT);
        assert.isNumber(imageInfo);
        assert.strictEqual(imageInfo, 10);
      });

    });

    it("should return CL_IMAGE_DEPTH", function () {
      U.withContext(function (context, device, platform) {
        var image = cl.createImage(context, 0, imageFormat, imageDesc, null);
        var imageInfo = f(image, cl.IMAGE_DEPTH);
        assert.isNumber(imageInfo);
      });

    });

    it("should return CL_IMAGE_ARRAY_SIZE", function () {
      U.withContext(function (context, device, platform) {
        var image = cl.createImage(context, 0, imageFormat, imageDesc, null);
        var imageInfo = f(image, cl.IMAGE_ARRAY_SIZE);
        assert.isNumber(imageInfo);
      });

    });

    it("should return CL_IMAGE_NUM_MIP_LEVELS", function () {
      U.withContext(function (context, device, platform) {
        var image = cl.createImage(context, 0, imageFormat, imageDesc, null);
        var imageInfo = f(image, cl.IMAGE_NUM_MIP_LEVELS);
        assert.isNumber(imageInfo);
      });

    });

    it("should return CL_IMAGE_NUM_SAMPLES", function () {
      U.withContext(function (context, device, platform) {
        var image = cl.createImage(context, 0, imageFormat, imageDesc, null);
        var imageInfo = f(image, cl.IMAGE_NUM_SAMPLES);
        assert.isNumber(imageInfo);
      });

    });

    it("should return CL_IMAGE_BUFFER", function () {
      U.withContext(function (context, device, platform) {
        var image = cl.createImage(context, 0, imageFormat, imageDesc, null);
        var imageInfo = f(image, cl.IMAGE_BUFFER);
        assert.isObject(imageInfo);
      });

    });

    it("should throw cl.INVALID_MEM_OBJECT if memory object is invalid", function () {
      U.withContext(function (context, device, platform) {
        f.bind(f, null, cl.IMAGE_BUFFER).should.throw(cl.INVALID_MEM_OBJECT.message);
      });
    });
  });
});
