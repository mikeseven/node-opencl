var cl = require('../lib/opencl');
var should = require('chai').should();
var chai = require('chai');
var util = require('util');
var assert = require('chai').assert;
var U = require("./utils/utils");
var log = console.log;
var skip = require("./utils/diagnostic");
var versions = require("./utils/versions");

versions(["2.0"]).describe("SVM", function() {

  describe("# SVMAlloc", function() {

    it("should return a valid buffer object", function() {
      U.withContext(function (ctx) {
        var buf = cl.SVMAlloc(ctx, 0, 200, null);

        assert.isObject(buf);
      });
    });

    it("should fail as context is invalid", function() {
      U.bind(cl.SVMAlloc(null, 0, 200, null).should.throw(cl.INVALID_CONTEXT.message);
    });
    
  });

  describe("# SVMFree", function() {

    it("should execute successfully when freeing an svm buffer", function() {
      U.withContext(function (ctx) {
        var buf = cl.SVMAlloc(ctx, 0, 200, null);
        cl.SVMFree(buf);
      });
    });

    it("should do nothing  when freeing a buffer which is not a SVM", function() {
      U.withContext(function (ctx) {
        cl.SVMFree(new Buffer(200));
      });
    });

    it("should do nothing when called twice", function(){
      U.withContext(function (ctx) {
        var buf = cl.SVMAlloc(ctx, 0, 200, null);
        cl.SVMFree(buf);
        cl.SVMFree(buf);
      });
    });
  });

  describe("# enqueueSVMFree", function() {
    it("should execute successfully when freeing a svm buffer", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.SVMAlloc(ctx, 0, 200, null);
          cl.enqueueSVMFree(cq, [buf]);
        });
      });
    });

    it("should free two buffers", function(){
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.SVMAlloc(ctx, 0, 200, null);
          var buf2 = cl.SVMAlloc(ctx, 0, 200, null);
          cl.enqueueSVMFree(cq, [buf, buf2]);
        });
      });
    });

    it("should call the given callback", function(done) {
      U.withAsyncContext(function (ctx, device, ctxDone) {
        U.withCQ(ctx, device, function (cq, cqDone) {
          var buf = cl.SVMAlloc(ctx, 0, 200, null);
          cl.enqueueSVMFree(cq, [buf], function() {
            ctxDone();
            cqDone();
            done();
          });
        });
      });
    });

    it("should fail as SVM buffers list is empty", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          U.bind(cl.enqueueSVMFree, cq, []).should.throw(cl.INVALID_VALUE);
        });
      });
    });
  });

  
  describe("# enqueueSVMMap", function() {
    it("should execute successfully when mapping a SVM buffer", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.SVMAlloc(ctx, 0, 200, null);
          cl.enqueueSVMMap(cq, true, 0, buf, 200);
        });
      });
    });

    it("should fail as buffer is null", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          U.bind(cl.enqueueSVMMap, cq, true, 0, null, 200).should.throw(cl.INVALID_VALUE.message);
        });
      });      
    });
  });

  
  describe("# enqueueSVMUnmap", function() {
    it("should execute successfully when unmapping a mapped SVM buffer", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.SVMAlloc(ctx, 0, 200, null);
          cl.enqueueSVMMap(cq, true, 0, buf, 200);
          cl.enqueueSVMUnmap(cq, buf);
          cl.finish();
        });
      });
    });

    it("should fail as buffer is null", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          U.bind(cl.enqueueSVMUnmap, cq, null).should.throw(cl.INVALID_VALUE);
        });
      });
    });
    
  });

  
  describe("# enqueueSVMMemcpy", function() {
    it("should update the destination SVM buffer", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.SVMAlloc(ctx, 0, 200, null);
          var ovv = new Buffer([0, 1, 2, 3, 4]);
          
          cl.enqueueSVMMemcpy(cq, true, 5, buf, ovv);

          assert(buf[0] == ovv[0]);
          assert(buf[1] == ovv[1]);
          assert(buf[2] == ovv[2]);
          assert(buf[3] == ovv[3]);
          assert(buf[4] == ovv[4]);
        });
      });
    });
    
  });

  
  describe("# enqueueSVMMemFill", function() {
    it("should set the buffer with the given pattern", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.SVMAlloc(ctx, 0, 200, null);
          
          cl.enqueueSVMMemFill(cq, buf, [2, 5], 2, 4);
          cl.finish();
          
          assert(buf[0] == 2);
          assert(buf[1] == 5);
          assert(buf[2] == 2);
          assert(buf[3] == 5);
        });
      });
    });

    it("should throw if size if not a multiple of pattern size", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.SVMAlloc(ctx, 0, 200, null);

          U.bind(cl.enqueueSVMMemFill, cq, buf, [2, 5], 2, 5)
            .should.throw(cl.INVALID_VALUE.message);
        });
      });      
    });
    
  });
  
});
