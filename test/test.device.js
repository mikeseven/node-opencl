var cl=require('../lib/opencl'),
  should=require('chai').should(),
  assert=require('chai').assert,
  util=require('util'),
  log=console.log;

describe("Device", function() {
  var platforms=cl.getPlatformIDs();
  var platform=platforms[0];
  var devices=cl.getDeviceIDs(platform);

  describe("#getDeviceIDs()",function() {
    it("should return an array",function() {
      var ids = cl.getDeviceIDs(platform);
      assert.isArray(ids);
      assert.isAbove(ids.length, 0);
    })
  });

  function testBoolean(device, name) {
    it(name+" should return a boolean",function(done) {
      var val=cl.getDeviceInfo(device,eval("cl."+name));
      assert.isBoolean(val);
      done(log(name+" = " + val))
    })
  }
  function testInteger(device, name) {
    it(name+" should return an integer",function(done) {
      var val=cl.getDeviceInfo(device,eval("cl."+name));
      assert.isNumber(val);
      done(log(name+" = " + val))
    })
  }
  function testString(device, name) {
    it(name+" should return a string",function(done) {
      var val=cl.getDeviceInfo(device,eval("cl."+name));
      assert.isString(val);
      done(log(name+" = " + val))
    })
  }
  function testObject(device, name) {
    it(name+" should return an object",function() {
      var info = cl.getDeviceInfo(device,eval("cl."+name));
      assert.isObject(info);
    })
  }
  function testArray(device, name) {
    it(name+" should return an array",function(done) {
      var val=cl.getDeviceInfo(device,eval("cl."+name));
      assert.isArray(val);
      done(log(name+" = " + val))
    })
  }

  function test64Array(device, name) {
    it(name+" should return a 2 integers array",function(done) {
      var val=cl.getDeviceInfo(device,eval("cl."+name));
      assert.isArray(val);
      assert.isNumber(val[0]);
      assert.isNumber(val[1]);
      done(log(name+" = " + val))
    })
  }

  function testDevice(device) {
    var device_vendor=cl.getDeviceInfo(device,cl.DEVICE_VENDOR);
    var device_name=cl.getDeviceInfo(device,cl.DEVICE_NAME);

    describe("#getDeviceInfo() for "+device_vendor+" "+device_name,function() {
      testString(device, "DEVICE_NAME");
      testString(device, "DEVICE_VENDOR");
      testString(device, "DEVICE_PROFILE");
      testString(device, "DEVICE_VERSION");
      testString(device, "DEVICE_OPENCL_C_VERSION");
      testString(device, "DEVICE_EXTENSIONS");
      var ext=cl.getDeviceInfo(device,cl.DEVICE_EXTENSIONS);
      var hasFP16=ext.toLowerCase().match(/cl_khr_fp16/g);
      var hasFP64=ext.toLowerCase().match(/cl_khr_fp64/g);
      testObject(device, "DEVICE_PLATFORM");
      testInteger(device, "DEVICE_TYPE");
      testInteger(device, "DEVICE_LOCAL_MEM_TYPE");
      testInteger(device, "DEVICE_GLOBAL_MEM_CACHE_TYPE");
      testInteger(device, "DEVICE_EXECUTION_CAPABILITIES");
      testInteger(device, "DEVICE_QUEUE_PROPERTIES");
      if(hasFP16) testInteger(device, "DEVICE_HALF_FP_CONFIG");
      testInteger(device, "DEVICE_SINGLE_FP_CONFIG");
      if(hasFP64) testInteger(device, "DEVICE_DOUBLE_FP_CONFIG");
      testArray(device, "DEVICE_MAX_WORK_ITEM_SIZES");
      testBoolean(device, "DEVICE_AVAILABLE");
      testBoolean(device, "DEVICE_COMPILER_AVAILABLE");
      testBoolean(device, "DEVICE_ENDIAN_LITTLE");
      testBoolean(device, "DEVICE_ERROR_CORRECTION_SUPPORT");
      testBoolean(device, "DEVICE_HOST_UNIFIED_MEMORY");
      testBoolean(device, "DEVICE_IMAGE_SUPPORT");

      testInteger(device, "DEVICE_ADDRESS_BITS");
      testInteger(device, "DEVICE_GLOBAL_MEM_CACHELINE_SIZE");
      testInteger(device, "DEVICE_MAX_CLOCK_FREQUENCY");
      testInteger(device, "DEVICE_MAX_COMPUTE_UNITS");
      testInteger(device, "DEVICE_MAX_CONSTANT_ARGS");
      testInteger(device, "DEVICE_MAX_READ_IMAGE_ARGS");
      testInteger(device, "DEVICE_MAX_SAMPLERS");
      testInteger(device, "DEVICE_MAX_WORK_ITEM_DIMENSIONS");
      testInteger(device, "DEVICE_MAX_WRITE_IMAGE_ARGS");
      testInteger(device, "DEVICE_MEM_BASE_ADDR_ALIGN");
      testInteger(device, "DEVICE_MIN_DATA_TYPE_ALIGN_SIZE");
      testInteger(device, "DEVICE_NATIVE_VECTOR_WIDTH_CHAR");
      testInteger(device, "DEVICE_NATIVE_VECTOR_WIDTH_SHORT");
      testInteger(device, "DEVICE_NATIVE_VECTOR_WIDTH_INT");
      testInteger(device, "DEVICE_NATIVE_VECTOR_WIDTH_LONG");
      testInteger(device, "DEVICE_NATIVE_VECTOR_WIDTH_FLOAT");
      testInteger(device, "DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE");
      testInteger(device, "DEVICE_NATIVE_VECTOR_WIDTH_HALF");
      testInteger(device, "DEVICE_PREFERRED_VECTOR_WIDTH_CHAR");
      testInteger(device, "DEVICE_PREFERRED_VECTOR_WIDTH_SHORT");
      testInteger(device, "DEVICE_PREFERRED_VECTOR_WIDTH_INT");
      testInteger(device, "DEVICE_PREFERRED_VECTOR_WIDTH_LONG");
      testInteger(device, "DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT");
      testInteger(device, "DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE");
      testInteger(device, "DEVICE_PREFERRED_VECTOR_WIDTH_HALF");
      testInteger(device, "DEVICE_VENDOR_ID");

      if(cl.CL_VERSION_1_2) {
        testInteger(device, "DEVICE_REFERENCE_COUNT");
        testInteger(device, "DEVICE_PARTITION_MAX_SUB_DEVICES");
      }

      test64Array(device, "DEVICE_GLOBAL_MEM_CACHE_SIZE");
      test64Array(device, "DEVICE_GLOBAL_MEM_SIZE");
      test64Array(device, "DEVICE_LOCAL_MEM_SIZE");
      test64Array(device, "DEVICE_MAX_CONSTANT_BUFFER_SIZE");
      test64Array(device, "DEVICE_MAX_MEM_ALLOC_SIZE");

      testInteger(device, "DEVICE_IMAGE2D_MAX_HEIGHT");
      testInteger(device, "DEVICE_IMAGE2D_MAX_WIDTH");
      testInteger(device, "DEVICE_IMAGE3D_MAX_DEPTH");
      testInteger(device, "DEVICE_IMAGE3D_MAX_HEIGHT");
      testInteger(device, "DEVICE_IMAGE3D_MAX_WIDTH");
      testInteger(device, "DEVICE_MAX_PARAMETER_SIZE");
      testInteger(device, "DEVICE_MAX_WORK_GROUP_SIZE");
      testInteger(device, "DEVICE_PROFILING_TIMER_RESOLUTION");

      if(cl.CL_VERSION_1_2) {
        testInteger(device, "DEVICE_IMAGE_MAX_BUFFER_SIZE");
        testInteger(device, "DEVICE_IMAGE_MAX_ARRAY_SIZE");
      }

      //// negative test cases
      it("should throw cl.INVALID_VALUE with name=-123.56",function() {
        cl.getDeviceInfo.bind(cl.getDeviceInfo,device,-123.56).should.throw(cl.INVALID_VALUE.message);
      });
      it("should throw cl.INVALID_VALUE with name='a string'",function() {
        cl.getDeviceInfo.bind(cl.getDeviceInfo,device,'a string').should.throw(cl.INVALID_VALUE.message);
      });
      it("should throw cl.INVALID_VALUE with name=123456",function() {
        cl.getDeviceInfo.bind(cl.getDeviceInfo,device,123456).should.throw(cl.INVALID_VALUE.message);
      });
      it("should throw cl.INVALID_DEVICE with device = null",function() {
        cl.getDeviceInfo.bind(cl.getDeviceInfo,null,123).should.throw(cl.INVALID_DEVICE.message);
      });
      it("should throw cl.INVALID_DEVICE with device = 'a string'",function() {
        cl.getDeviceInfo.bind(cl.getDeviceInfo,'a string',123).should.throw(cl.INVALID_DEVICE.message);
      });
      it("should throw cl.INVALID_DEVICE with device = 123",function() {
        cl.getDeviceInfo.bind(cl.getDeviceInfo,123,123).should.throw(cl.INVALID_DEVICE.message);
      });
      it("should throw cl.INVALID_DEVICE with device = [1,2,3]",function() {
        cl.getDeviceInfo.bind(cl.getDeviceInfo,[1,2,3],123).should.throw(cl.INVALID_DEVICE.message);
      });
      it("should throw cl.INVALID_DEVICE with device = new Array()",function() {
        cl.getDeviceInfo.bind(cl.getDeviceInfo,[],123).should.throw(cl.INVALID_DEVICE.message);
      })

    });
    describe("#createSubDevices() for "+device_vendor+" "+device_name,function() {
    });
    describe("#retainDevice() for "+device_vendor+" "+device_name,function() {
      it("should increase device reference count",function() {
        cl.retainDevice(device).should.equal(0);
      })
    });
    describe("#releaseDevice() for "+device_vendor+" "+device_name,function() {
      it("should decrease device reference count",function() {
        cl.releaseDevice(device).should.equal(0);
      })
    })
  }

  testDevice(global.MAIN_DEVICE_ID);

});
