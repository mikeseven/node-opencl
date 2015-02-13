var cl=require('../lib/opencl'),
    should=require('chai').should(),
    util=require('util'),
    testUtils = require("../lib/test_utils"),
    log=console.log,
    assert=require("chai").assert,
    fs = require("fs");


var squareKern = fs.readFileSync(__dirname + "/kernels/square.cl").toString();

describe("Program", function() {

  describe("#createProgramWithSource", function() {

    it("should return a valid program", function () {
      testUtils.withContext(function(ctx){
        var prg = cl.createProgramWithSource(ctx, squareKern);

        assert.isNotNull(prg);
        assert.isDefined(prg);

        cl.releaseProgram(prg);
      });
    });

    it("should throw as context is invalid", function () {
        testUtils.bind(cl.createProgramWithSource, null, squareKern)
            .should.throw(cl.INVALID_CONTEXT);
    });

  });

  describe("#buildProgram", function() {

    it("should build using a valid program and a given device", function() {
      testUtils.withContext(function(ctx, device){
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.buildProgram(prg, [device]);
        assert(ret == cl.SUCCESS);

        cl.releaseProgram(prg);
      });
    });

    it("should build using a valid program", function() {
      testUtils.withContext(function(ctx){
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.buildProgram(prg);
        assert(ret == cl.SUCCESS);

        cl.releaseProgram(prg);
      });
    });

    it("should build using a valid program and options", function() {
      testUtils.withContext(function(ctx){
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.buildProgram(prg, null, "-D NOCL_TEST=5");
        assert(ret == cl.SUCCESS);

        cl.releaseProgram(prg);
      });
    });


    it("should throw if program is NULL", function() {
      testUtils.withContext(function(ctx){
        testUtils.bind(cl.buildProgram, null)
            .should.throw(cl.INVALID_PROGRAM);
      });
    });

    it("should throw if program is INVALID", function() {
      testUtils.withContext(function(ctx){
        var prg = cl.createProgramWithSource(ctx, squareKern + "$bad_inst");
        testUtils.bind(cl.buildProgram, prg)
            .should.throw(cl.INVALID_PROGRAM);
        cl.releaseProgram(prg);
      });
    });

  });


  describe("#createProgramWithBinary", function() {

  });
  describe("#createProgramWithBuiltInKernels", function() {
      // TODO
  });
  describe("#retainProgram", function() {

  });
  describe("#releaseProgram", function() {

  });
  describe("#compileProgram", function() {

  });
  describe("#linkProgram", function() {

  });
  describe("#unloadPlatformCompiler", function() {

  });
  describe("#getProgramInfo", function() {

  });
  describe("#getProgramBuildInfo", function() {

  });
})

describe("Kernel", function() {
  describe("#createKernel", function() {

  });
  describe("#createKernelsInProgram", function() {

  });
  describe("#retainKernel", function() {

  });
  describe("#releaseKernel", function() {

  });
  describe("#setKernelArg", function() {

  });
  describe("#getKernelInfo", function() {

  });
  describe("#getKernelArgInfo", function() {

  });
  describe("#getKernelWorkGroupInfo", function() {

  });
})
