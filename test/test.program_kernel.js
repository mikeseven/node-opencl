var cl = require('../lib/opencl'),
  should = require('chai').should(),
  util = require('util'),
  testUtils = require("../lib/test_utils"),
  log = console.log,
  assert = require("chai").assert,
  fs = require("fs");


var squareKern = fs.readFileSync(__dirname + "/kernels/square.cl").toString();
var squareCpyKern = fs.readFileSync(__dirname + "/kernels/square_cpy.cl").toString();

describe("Program", function () {

  describe("#createProgramWithSource", function () {

    it("should return a valid program", function () {
      testUtils.withContext(function (ctx) {
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

  describe("#buildProgram", function () {

    it("should build using a valid program and a given device", function () {
      testUtils.withContext(function (ctx, device) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.buildProgram(prg, [device]);
        assert(ret == cl.SUCCESS);

        cl.releaseProgram(prg);
      });
    });

    it("should build using a valid program", function () {
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.buildProgram(prg);
        assert(ret == cl.SUCCESS);

        cl.releaseProgram(prg);
      });
    });

    it("should build using a valid program and options", function () {
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.buildProgram(prg, null, "-D NOCL_TEST=5");
        assert(ret == cl.SUCCESS);

        cl.releaseProgram(prg);
      });
    });


    it("should throw if program is NULL", function () {
      testUtils.withContext(function (ctx) {
        testUtils.bind(cl.buildProgram, null)
          .should.throw(cl.INVALID_PROGRAM);
      });
    });

    it("should throw if program is INVALID", function () {
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern + "$bad_inst");
        testUtils.bind(cl.buildProgram, prg)
          .should.throw(cl.INVALID_PROGRAM);
      });
    });

  });


  describe("#createProgramWithBinary", function () {

    it("should create a valid program from a binary", function () {
      assert(false, "This one segfaults (OSX) == disabled");
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var bin = cl.getProgramInfo(prg, cl.PROGRAM_BINARIES);
        var sizes = cl.getProgramInfo(prg, cl.PROGRAM_BINARY_SIZES);

        var prg2 = cl.createProgramFromBinary(ctx, null, sizes, bin);

        assert.isNotNull(prg2);
        assert.isDefined(prg2);

        cl.releaseProgram(prg);
      });
    });

    it("should fail as program is invalid", function () {
      assert(false, "This one segfaults == disabled");
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern + "$bad_inst");
        var bin = cl.getProgramInfo(prg, cl.PROGRAM_BINARIES);
        var sizes = cl.getProgramInfo(prg, cl.PROGRAM_BINARY_SIZES);
        var prg2 = cl.createProgramWithBinary(ctx, null, sizes, bin);

        testUtils.bind(cl.createProgramFronBinary, ctx, null, sizes, bin)
          .should.throw(cl.INVALID_PROGRAM);
      });
    });

  });


  describe("#createProgramWithBuiltInKernels", function () {

  });

  describe("#retainProgram", function () {
    it("should increment the reference count", function () {
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var before = cl.getProgramInfo(prg, cl.PROGRAM_REFERENCE_COUNT);
        cl.retainProgram(prg);
        var after = cl.getProgramInfo(prg, cl.PROGRAM_REFERENCE_COUNT);
        assert(before + 1 == after);
        cl.releaseProgram(prg);
      });
    });
  });

  describe("#releaseProgram", function () {
    it("should decrement the reference count", function () {
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var before = cl.getProgramInfo(prg, cl.PROGRAM_REFERENCE_COUNT);
        cl.retainProgram(prg);
        cl.releaseProgram(prg);
        var after = cl.getProgramInfo(prg, cl.PROGRAM_REFERENCE_COUNT);
        assert(before == after);
        cl.releaseProgram(prg);
      });
    });
  });

  describe("#compileProgram", function () {
    it("should build a program with no input headers", function () {
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.compileProgram(prg);
        assert(ret == cl.SUCCESS);
        cl.releaseProgram(prg);
      });
    });

    it("should build a program with an input header", function () {
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var prg2 = cl.createProgramWithSource(ctx, squareKern);

        var ret = cl.compileProgram(prg, null, null, [prg2], ["prg2.h"]);
        assert(ret == cl.SUCCESS);
        cl.releaseProgram(prg);
      });
    });

    it("should fail as ain't no name for header", function () {
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var prg2 = cl.createProgramWithSource(ctx, squareKern);

        testUtils.bind(cl.compileProgram, prg, null, null, [prg2], [])
          .should.throw();

        cl.releaseProgram(prg);
        cl.releaseProgram(prg2);
      });
    });
  });

  describe("#linkProgram", function () {
    it("should link a single program", function () {
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var prgl = cl.linkProgram(prg, null, null, 1, [prg]);
        assert.isNotNull(prgl);
        assert.isDefined(prgl);
        cl.releaseProgram(prg);
      });
    });

    it("should link two programs", function () {
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var prg2 = cl.createProgramWithSource(ctx, squareKern);
        cl.linkProgram(prg, null, null, 1, [prg, prg2]);
        assert.isNotNull(prgl);
        assert.isDefined(prgl);
        cl.releaseProgram(prg);
      });
    });

    it("should fail as a program is invalid", function () {
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern + "$bad_inst");
        testUtils.bind(cl.linkProgrram, ctx, null, null, 1, [prg])
          .should.throw(cl.INVALID_PROGRAM);
        cl.releaseProgram(prg);
      });
    });
  });

  describe("#unloadPlatformCompiler", function () {
    it("should work when using a valid platform", function() {
      testUtils.withContext(function (ctx, device, platform) {
        cl.unloadPlatformCompiler(platform);
      })
    });
  });

  describe("#getProgramInfo", function () {
    testUtils.withContext(function (ctx) {
      it("should have the right types for properties", function () {
        testUtils.withContext(function (ctx) {
          var prg = cl.createProgramWithSource(ctx, squareKern);
          cl.getProgramInfo(prg, cl.PROGRAM_REFERENCE_COUNT).should.be.a.integer;
          cl.getProgramInfo(prg, cl.PROGRAM_NUM_DEVICES).should.be.a.integer;
          cl.getProgramInfo(prg, cl.PROGRAM_CONTEXT).should.be.a.array;
          cl.getProgramInfo(prg, cl.PROGRAM_DEVICES).should.be.a.array;
          cl.getProgramInfo(prg, cl.PROGRAM_BINARY_SIZES).should.be.a.array;
          cl.getProgramInfo(prg, cl.PROGRAM_SOURCE).should.be.a.string;
          cl.getProgramInfo(prg, cl.PROGRAM_KERNEL_NAMES).should.be.a.string;
          cl.releaseProgram(prg);
        });
      });

      it("should have the same program source as the one given", function () {
        testUtils.withContext(function (ctx) {
          var prg = cl.createProgramWithSource(ctx, squareKern);
          assert(cl.getProgramInfo(prg, cl.PROGRAM_SOURCE) == squareKern);
          cl.releaseProgram(prg);
        });

      });

    });
  });

  describe("#getProgramBuildInfo", function () {

    it("should have the right types for properties", function () {
      testUtils.withContext(function (ctx, device) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.buildProgram(prg);
        cl.getProgramBuildInfo(prg, device, cl.PROGRAM_BUILD_STATUS).should.be.a.integer;
        cl.getProgramBuildInfo(prg, device, cl.PROGRAM_BUILD_OPTIONS).should.be.a.string;
        cl.getProgramBuildInfo(prg, device, cl.PROGRAM_BUILD_LOG).should.be.a.string;
        cl.getProgramBuildInfo(prg, device, cl.PROGRAM_BINARY_TYPE).should.be.a.integer;

        cl.releaseProgram(prg);
      });
    });

    it("should return the same options string that was passed before", function () {
      testUtils.withContext(function (ctx, device) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var buildOpts = "-D NOCL_TEST=5";
        var ret = cl.buildProgram(prg, null, buildOpts);

        var opt = cl.getProgramBuildInfo(prg, device, cl.PROGRAM_BUILD_OPTIONS);
        assert(opt.indexOf(buildOpts) !== -1); // there is an extra space in get info output
        cl.releaseProgram(prg);
      });
    });

  });

})


// -------------------------

describe("Kernel", function () {

  describe("#createKernel", function () {

    it("should return a valid kernel", function () {
      testUtils.withContext(function(ctx){
        testUtils.withProgram(ctx, squareKern, function(prg){
          var k = cl.createKernel(prg, "square");

          assert.isNotNull(k);
          assert.isDefined(k);
          cl.releaseKernel(k);
        });
      });
    });

    it("should fail as kernel does not exists", function () {
      testUtils.withContext(function (ctx) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          testUtils.bind(cl.createKernel, prg, "i_do_not_exist").should.throw();
        });
      });
    });

  });


  describe("#createKernelsInProgram", function () {

    it("should return two valid kernels", function() {
      testUtils.withContext(function (ctx) {
        testUtils.withProgram(ctx, [squareKern, squareCpyKern].join("\n"), function (prg) {
          var kerns = cl.createKernelsInProgram(prg, 2);

          assert.isNotNull(kerns);
          assert.isDefined(kerns);
          assert(kerns.length == 2);
          assert.isNotNull(kerns[0]);
          assert.isDefined(kerns[0]);
          assert.isNotNull(kerns[1]);
          assert.isDefined(kerns[1]);

          cl.releaseKernel(kerns[0]);
          cl.releaseKernel(kerns[1]);
        });
      });
    });

  });


  describe("#retainKernel", function () {

    it("should increment reference count", function () {
      testUtils.withContext(function (ctx) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");

          var before = cl.getKernelInfo(k, cl.KERNEL_REFERENCE_COUNT);
          cl.retainKernel(k);
          var after = cl.getKernelInfo(k, cl.KERNEL_REFERENCE_COUNT);
          assert(before + 1 == after);
          cl.releaseKernel(k);
        });
      });
    });

  });

  describe("#releaseKernel", function () {

    it("should decrement reference count", function () {
      testUtils.withContext(function (ctx) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var before = cl.getKernelInfo(k, cl.KERNEL_REFERENCE_COUNT);

          cl.retainKernel(k);
          cl.releaseKernel(k);
          var after = cl.getKernelInfo(k, cl.KERNEL_REFERENCE_COUNT);

          //assert(before == after);
          cl.releaseKernel(k);
        });
      });
    })

  });

  describe("#setKernelArg", function () {

    it("should be implemented", function () {
      // TODO
      assert(false);
    });

  });

  describe("#getKernelInfo", function () {

    it("should have valid types for properties", function () {
      testUtils.withContext(function (ctx) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");

          cl.getKernelInfo(k, cl.KERNEL_ATTRIBUTES).should.be.a.string;
          cl.getKernelInfo(k, cl.KERNEL_FUNCTION_NAME).should.be.a.string;
          cl.getKernelInfo(k, cl.KERNEL_REFERENCE_COUNT).should.be.a.integer;
          cl.getKernelInfo(k, cl.KERNEL_NUM_ARGS).should.be.a.integer;

          var c = cl.getKernelInfo(k, cl.KERNEL_CONTEXT);
          var p = cl.getKernelInfo(k, cl.KERNEL_PROGRAM);

          assert.isNotNull(c);
          assert.isDefined(c);

          assert.isNotNull(p);
          assert.isDefined(c);
        });
      });
    });

    it("should return the corresponding number of arguments", function () {
      testUtils.withContext(function (ctx) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var nb_args = cl.getKernelInfo(k, cl.KERNEL_NUM_ARGS);

          if (nb_args != 3) {
            assert.fail(nb_args, 3);
          }
        });
      });
    });

    it("should return the corresponding kernel name", function () {
      testUtils.withContext(function (ctx) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var name = cl.getKernelInfo(k, cl.KERNEL_FUNCTION_NAME);

          if (name != "square") {
            assert.fail(name, "square");
          }
        });
      });
    });

    it("should return the corresponding context", function () {
      testUtils.withContext(function (ctx) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var c = cl.getKernelInfo(k, cl.KERNEL_CONTEXT);

          assert(c == ctx);
        });
      });
    });

    it("should return the corresponding program", function () {
      testUtils.withContext(function (ctx) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var p = cl.getKernelInfo(k, cl.KERNEL_PROGRAM);

          assert(p == prg);
        });
      });
    });

  });

  describe("#getKernelArgInfo", function () {
    it("should have valid types for properties", function(){
      testUtils.withContext(function (ctx, device) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");

          cl.getKernelArgInfo(k, 0, cl.KERNEL_ARG_ADDRESS_QUALIFIER).should.be.a.integer;
          cl.getKernelArgInfo(k, 0, cl.KERNEL_ARG_ACCESS_QUALIFIER).should.be.a.integer;
          cl.getKernelArgInfo(k, 0, cl.KERNEL_ARG_TYPE_QUALIFIER).should.be.a.integer;

          cl.getKernelArgInfo(k, 0, cl.KERNEL_ARG_TYPE_NAME).should.be.a.string;
          cl.getKernelArgInfo(k, 0, cl.KERNEL_ARG_NAME).should.be.a.string;
        });
      });
    })

    it("should return the corresponding names", function(){
      testUtils.withContext(function (ctx) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var n1 = cl.getKernelArgInfo(k, 0, cl.KERNEL_ARG_TYPE_NAME);
          var n2 = cl.getKernelArgInfo(k, 1, cl.KERNEL_ARG_TYPE_NAME);
          var n3 = cl.getKernelArgInfo(k, 2, cl.KERNEL_ARG_TYPE_NAME);

          assert.equal(n1, "input");
          assert.equal(n2, "output");
          assert.equal(n3, "count");
        });
      });
    });


    it("should return the corresponding types", function(){
      testUtils.withContext(function (ctx) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var n1 = cl.getKernelArgInfo(k, 0, cl.KERNEL_ARG_TYPE_NAME);
          var n2 = cl.getKernelArgInfo(k, 1, cl.KERNEL_ARG_TYPE_NAME);
          var n3 = cl.getKernelArgInfo(k, 2, cl.KERNEL_ARG_TYPE_NAME);

          assert.equal(n1, "float*");
          assert.equal(n2, "float*");
          assert.equal(n3, "int");
        });
      });
    });
  });

  describe("#getKernelWorkGroupInfo", function () {

    it("should have valid types for properties", function(){
      testUtils.withContext(function (ctx, device) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");

          cl.getKernelWorkGroupInfo(k, device, cl.KERNEL_COMPILE_WORK_GROUP_SIZE).should.be.a.array;
          cl.getKernelWorkGroupInfo(k, device, cl.KERNEL_GLOBAL_WORK_SIZE).should.be.a.array;

          cl.getKernelWorkGroupInfo(k, device, cl.KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE).should.be.a.integer;
          cl.getKernelWorkGroupInfo(k, device, cl.KERNEL_WORK_GROUP_SIZE).should.be.a.integer;

          cl.getKernelWorkGroupInfo(k, device, cl.KERNEL_LOCAL_MEM_SIZE).should.be.a.integer;
          cl.getKernelWorkGroupInfo(k, device, cl.KERNEL_PRIVATE_MEM_SIZE).should.be.a.integer;
        });
      });
    })
  });

});
