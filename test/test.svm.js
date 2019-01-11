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

        assert.ok(buf);
      });
    });

    it("should fail as context is invalid", function() {
      U.bind(cl.SVMAlloc,null, 0, 200, null).should.throw(cl.INVALID_CONTEXT.message);
    });
    
  });

  describe("# SVMFree", function() {

    it("should execute successfully when freeing an svm buffer", function() {
      U.withContext(function (ctx) {
        var buf = cl.SVMAlloc(ctx, 0, 200, null);
        cl.SVMFree(ctx,buf);
        assert.isUndefined(buf[0]);
      });
    });

    it("should do nothing  when freeing a buffer which is not a SVM", function() {
      U.withContext(function (ctx) {
        U.bind(cl.SVMFree,ctx,new Buffer(200)).should.not.throw(cl.INVALID_ARG_VALUE.message);
      });
    });

    it("should do nothing when called twice", function(){
      U.withContext(function (ctx) {
        var buf = cl.SVMAlloc(ctx, 0, 200, null);
        cl.SVMFree(ctx,buf);
        U.bind(cl.SVMFree,ctx,buf).should.not.throw(cl.INVALID_ARG_VALUE.message);
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
      U.withAsyncContext(function (ctx, device,platform, ctxDone) {
        U.withAsyncCQ(ctx, device, function (cq, cqDone) {
          var buf = cl.SVMAlloc(ctx, 0, 200, null);
          cl.enqueueSVMFree(cq, [buf], function() {
            cqDone();
            ctxDone();
            done();
          });
        });
      });
    });

    it("should fail as SVM buffers list is empty", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          U.bind(cl.enqueueSVMFree, cq, []).should.throw(/*..*/);
        });
      });
    });
  });

  
  describe("# enqueueSVMMap", function() {
    it("should execute successfully when mapping a SVM buffer", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.SVMAlloc(ctx, 0, 200, 0);
          cl.enqueueSVMMap(cq, true, cl.MAP_READ, buf, 200);

        });
      });
    });

    it("should fail as buffer is null", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          U.bind(cl.enqueueSVMMap, cq, true, 0, null, 200).should.throw("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
        });
      });      
    });
  });

  
  describe("# enqueueSVMUnmap", function() {
    it("should execute successfully when unmapping a mapped SVM buffer", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.SVMAlloc(ctx, 0, 200, null);
          cl.enqueueSVMMap(cq, true, cl.MAP_READ, buf, 200);
          cl.enqueueSVMUnmap(cq, buf);
          cl.finish(cq);
        });
      });
    });

    it("should fail as buffer is null", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          U.bind(cl.enqueueSVMUnmap, cq, null).should.throw(/*..*/);
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

          cl.enqueueSVMMemcpy(cq, true, buf, ovv, 5);
          cl.finish(cq);
          
          buf = Buffer.from(buf);
          assert.equal(buf[0], ovv[0], 'buf[0]');
          assert.equal(buf[1], ovv[1], 'buf[1]');
          assert.equal(buf[2], ovv[2], 'buf[2]');
          assert.equal(buf[3], ovv[3], 'buf[3]');
          assert.equal(buf[4], ovv[4], 'buf[4]');
        });
      });
    });
    
  });

  
  describe("# enqueueSVMMemFill", function() {
    it.skip("should set the buffer with the given pattern", function() {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.SVMAlloc(ctx, cl.MEM_READ_WRITE, 160, 16);
          var ovv = new Buffer(16);
          
          cl.enqueueSVMMemFill(cq, buf, ovv, 16);

          cl.enqueueSVMMap(cq,true,cl.MAP_READ,buf,200);
          cl.finish(cq);
          
          buf = Buffer.from(buf);
          assert.equal(buf[0], 0, 'buf[0]');
          assert.equal(buf[1], 1, 'buf[1]');
          assert.equal(buf[2], 2, 'buf[2]');
          assert.equal(buf[3], 3, 'buf[3]');

          cl.enqueueSVMUnmap(cq,buf);
          cl.finish(cq);
        });
      });
    });

    it("should throw if size if not a multiple of pattern size", function () {
      U.withContext(function (ctx, device) {
        U.withCQ(ctx, device, function (cq) {
          var buf = cl.SVMAlloc(ctx, 0, 200, null);
          var ovv = new Buffer([0, 1, 2, 3, 4]);

          U.bind(cl.enqueueSVMMemFill, cq, buf, ovv, ovv.length)
            .should.throw(cl.INVALID_VALUE.message);
        });
      });      
    });
    
  });
  
});
