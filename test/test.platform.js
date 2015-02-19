var cl = require('../lib/opencl');
var should = require('chai').should();
var assert = require('chai').assert;
var util = require('util');
var log = console.log;

describe("Platform", function() {
  var platforms=cl.getPlatformIDs();

  describe("#getPlatformIDs()",function() {
    it("should return an array",function() {
      var devices = cl.getPlatformIDs();
      assert.isArray(devices);
      assert.isAbove(devices.length, 0);
    })
  });

  function testString(platform, name) {
    it(name+" should return a string",function(done) {
      var val=cl.getPlatformInfo(platform,eval("cl."+name));
      assert.isString(val);
      done(log(name+" = " + val))
    })
  }

  function testPlatform(p) {
    describe("#getPlatformInfo() for "+cl.getPlatformInfo(p,cl.PLATFORM_VENDOR)+" "+cl.getPlatformInfo(p,cl.PLATFORM_NAME),function() {
      testString(p, "PLATFORM_VERSION");
      testString(p, "PLATFORM_PROFILE");
      testString(p, "PLATFORM_NAME");
      testString(p, "PLATFORM_VENDOR");
      testString(p, "PLATFORM_EXTENSIONS");

      // negative test cases
      it("should throw cl.INVALID_VALUE with name=cl.DEVICE_TYPE_CPU",function() {
        cl.getPlatformInfo.bind(cl.getPlatformInfo,p,cl.DEVICE_TYPE_CPU).should.throw(cl.INVALID_VALUE.message);
      });
      it("should throw cl.INVALID_VALUE with name=0x2000",function() {
        cl.getPlatformInfo.bind(cl.getPlatformInfo,p,0x2000).should.throw(cl.INVALID_VALUE.message);
      });
      it("should throw cl.INVALID_VALUE with name='a string'",function() {
        cl.getPlatformInfo.bind(cl.getPlatformInfo,p,'a string').should.throw(cl.INVALID_VALUE.message);
      });
      it("should throw cl.INVALID_VALUE with name=-123.56",function() {
        cl.getPlatformInfo.bind(cl.getPlatformInfo,p,-123.56).should.throw(cl.INVALID_VALUE.message);
      });
      it("should throw cl.INVALID_PLATFORM with platform = null",function() {
        cl.getPlatformInfo.bind(cl.getPlatformInfo,null,123).should.throw(cl.INVALID_PLATFORM.message);
      });
      it("should throw cl.INVALID_PLATFORM with platform = 'a string'",function() {
        cl.getPlatformInfo.bind(cl.getPlatformInfo,'a string',123).should.throw(cl.INVALID_PLATFORM.message);
      });
      it("should throw cl.INVALID_PLATFORM with platform = 123",function() {
        cl.getPlatformInfo.bind(cl.getPlatformInfo,123,123).should.throw(cl.INVALID_PLATFORM.message);
      });
      it("should throw cl.INVALID_PLATFORM with platform = [1,2,3]",function() {
        cl.getPlatformInfo.bind(cl.getPlatformInfo,[1,2,3],123).should.throw(cl.INVALID_PLATFORM.message);
      });
      it("should throw cl.INVALID_PLATFORM with platform = new Array()",function() {
        cl.getPlatformInfo.bind(cl.getPlatformInfo,[],123).should.throw(cl.INVALID_PLATFORM.message);
      })
    })
  }

  for(var i=0;i<platforms.length;i++)
    testPlatform(platforms[i]);
});

