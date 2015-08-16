var cl = require('../lib/opencl');
var should = require('chai').should();
var assert = require('chai').assert;
var util = require('util');
var log = console.log;
var skip = require("./utils/diagnostic");
var U = require("./utils/utils.js");
var versions = require("./utils/versions");

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
      testString(device, "DEVICE_BUILT_IN_KERNELS");
      testString(device, "DEVICE_SPIR_VERSIONS");
      testString(device, "DRIVER_VERSION");
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
      testBoolean(device, "DEVICE_LINKER_AVAILABLE");
      testBoolean(device, "DEVICE_PREFERRED_INTEROP_USER_SYNC");


      testInteger(device, "DEVICE_IMAGE_PITCH_ALIGNMENT");
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
      testInteger(device, "DEVICE_MAX_GLOBAL_VARIABLE_SIZE");
      testInteger(device, "DEVICE_MAX_ON_DEVICE_EVENTS");
      testInteger(device, "DEVICE_MAX_ON_DEVICE_QUEUES");

      if(cl.CL_VERSION_1_2) {
        testInteger(device, "DEVICE_REFERENCE_COUNT");
        testInteger(device, "DEVICE_PARTITION_MAX_SUB_DEVICES");
      }

      test64Array(device, "DEVICE_GLOBAL_MEM_CACHE_SIZE");
      test64Array(device, "DEVICE_GLOBAL_MEM_SIZE");
      test64Array(device, "DEVICE_LOCAL_MEM_SIZE");
      test64Array(device, "DEVICE_MAX_CONSTANT_BUFFER_SIZE");
      test64Array(device, "DEVICE_MAX_MEM_ALLOC_SIZE");

      testInteger(device, "DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE");
      testInteger(device, "DEVICE_PRINTF_BUFFER_SIZE");
      testInteger(device, "DEVICE_IMAGE2D_MAX_HEIGHT");
      testInteger(device, "DEVICE_IMAGE2D_MAX_WIDTH");
      testInteger(device, "DEVICE_IMAGE3D_MAX_DEPTH");
      testInteger(device, "DEVICE_IMAGE3D_MAX_HEIGHT");
      testInteger(device, "DEVICE_IMAGE3D_MAX_WIDTH");
      testInteger(device, "DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT");
      testInteger(device, "DEVICE_MAX_PARAMETER_SIZE");
      testInteger(device, "DEVICE_MAX_WORK_GROUP_SIZE");
      testInteger(device, "DEVICE_PROFILING_TIMER_RESOLUTION");
      testInteger(device, "DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT");
      testInteger(device, "DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT");
      testInteger(device, "DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT");
      testInteger(device, "DEVICE_QUEUE_ON_DEVICE_MAX_SIZE");
      testInteger(device, "DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE");

      if (U.checkVersion("2.x")) {
        testInteger(device, "DEVICE_PIPE_MAX_PACKET_SIZE");
        testInteger(device, "DEVICE_MAX_PIPE_ARGS");
        testInteger(device, "DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS");
      }

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

    versions(["1.2", "2.0"]).describe("#createSubDevices() for "+device_vendor+" "+device_name,function() {

      skip().vendor("Intel").it("should return an array of sub-devices", function() {

        var subdevices;
        try {
          cl.createSubDevices(device, [cl.DEVICE_PARTITION_BY_COUNTS, 3, 1, cl.DEVICE_PARTITION_BY_COUNTS_LIST_END, 0], 2);
          assert.isArray(subDevices);
          assert.isAbove(subDevices.length, 0);
        } catch (error) {
          if (error.message === cl.DEVICE_PARTITION_FAILED.message) {
            assert.isTrue(true);
          }
        }

      })

      skip().vendor("Intel").it("should return an array of sub-devices", function() {

        var subdevices;
        try {
          cl.createSubDevices(device, [cl.DEVICE_PARTITION_EQUALLY, 8, 0], 2);
          assert.isArray(subDevices);
          assert.isAbove(subDevices.length, 0);
        } catch (error) {
          if (error.message === cl.DEVICE_PARTITION_FAILED.message) {
            assert.isTrue(true);
          }
        }

      })

      skip().vendor("Intel").it("should return an array of sub-devices", function() {

        var subdevices;
        try {
          cl.createSubDevices(device, [cl.DEVICE_PARTITION_BY_AFFINITY_DOMAIN, cl.DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE, 0], 2);
          assert.isArray(subDevices);
          assert.isAbove(subDevices.length, 0);
        } catch (error) {
          if (error.message === cl.DEVICE_PARTITION_FAILED.message) {
            assert.isTrue(true);
          }
        }

      })

    });
    versions(["1.2","2.0"]).describe("#retainDevice() for "+device_vendor+" "+device_name,function() {

      var f = cl.retainDevice;

      it("should throw cl.INVALID_DEVICE if device is not a subdevice", function () {
        f.bind(f, device).should.throw(cl.INVALID_DEVICE.message);
      });

      it("should increase device reference count",function() {
        try {
          var subDevice = cl.createSubDevices(device, cl.DEVICE_PARTITION_BY_COUNTS, 2);
          cl.retainDevice(subDevice);
          var count = cl.getDeviceInfo(subDevice, cl.DEVICE_REFERENCE_COUNT);
          assert.strictEqual(count, 2);
          cl.releaseDevice(subDevice);
        } catch (error) {
          if (error.message === cl.DEVICE_PARTITION_FAILED.message) {
            assert.isTrue(true);
          }
        }

      })
    });
    versions(["1.2","2.0"]).describe("#releaseDevice() for "+device_vendor+" "+device_name,function() {

      var f = cl.releaseDevice;

      it("should throw cl.INVALID_DEVICE if device is not a subdevice", function () {
        f.bind(f, device).should.throw(cl.INVALID_DEVICE.message);
      });

      it("should decrease device reference count",function() {
        try {
          var subDevice = cl.createSubDevices(device, cl.DEVICE_PARTITION_BY_COUNTS, 2);
          cl.retainDevice(subDevice);
          var count = cl.getDeviceInfo(subDevice, cl.DEVICE_REFERENCE_COUNT);
          assert.strictEqual(count, 2);
          cl.releaseDevice(subDevice);
          count = cl.getDeviceInfo(subDevice, cl.DEVICE_REFERENCE_COUNT);
          assert.strictEqual(count, 1);
        } catch (error) {
          if (error.message === cl.DEVICE_PARTITION_FAILED.message) {
            assert.isTrue(true);
          }
        }
      })
    })
  }

  testDevice(global.MAIN_DEVICE_ID);

});
