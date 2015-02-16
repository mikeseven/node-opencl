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
        .should.throw(cl.INVALID_CONTEXT.message);
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
          .should.throw(cl.INVALID_PROGRAM.message);
      });
    });

    it("should throw if program is INVALID", function () {
      testUtils.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern + "$bad_inst");
        testUtils.bind(cl.buildProgram, prg)
          .should.throw(cl.BUILD_PROGRAM_FAILURE.message);
      });
    });

  });


  describe("#createProgramWithBinary", function () {

    it("should create a valid program from a binary", function () {

      testUtils.withContext(function (ctx, device) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        cl.buildProgram(prg);
        var bin = cl.getProgramInfo(prg, cl.PROGRAM_BINARIES);
        var sizes = cl.getProgramInfo(prg, cl.PROGRAM_BINARY_SIZES);
        //
        var prg2 = cl.createProgramWithBinary(ctx, [device], sizes, bin);

        assert.isNotNull(prg2);
        assert.isDefined(prg2);

        cl.releaseProgram(prg);
      });
    });

    it("should fail as binaries list is empty", function () {
      testUtils.withContext(function (ctx, device) {
        testUtils.bind(cl.createProgramWithBinary, ctx, [device], [], [])
          .should.throw(cl.INVALID_VALUE.message);
      });
    });


    it("should fail as lists are not of the same length", function () {

      testUtils.withContext(function (ctx, device) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        cl.buildProgram(prg);
        var bin = cl.getProgramInfo(prg, cl.PROGRAM_BINARIES);
        var sizes = cl.getProgramInfo(prg, cl.PROGRAM_BINARY_SIZES);
        sizes.push(100);

        testUtils.bind(cl.createProgramWithBinary, ctx, [device], sizes, bin)
        cl.releaseProgram(prg);
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
    it("should be implemented", function() {
      // TODO
    });
    //it("should link a single program", function () {
    //  testUtils.withContext(function (ctx) {
    //    var prg = cl.createProgramWithSource(ctx, squareKern);
    //    var prgl = cl.linkProgram(prg, null, null, 1, [prg]);
    //    assert.isNotNull(prgl);
    //    assert.isDefined(prgl);
    //    cl.releaseProgram(prg);
    //  });
    //});
    //
    //it("should link two programs", function () {
    //  testUtils.withContext(function (ctx) {
    //    var prg = cl.createProgramWithSource(ctx, squareKern);
    //    var prg2 = cl.createProgramWithSource(ctx, squareKern);
    //    var prgl = cl.linkProgram(prg, null, null, 1, [prg, prg2]);
    //    assert.isNotNull(prgl);
    //    assert.isDefined(prgl);
    //    cl.releaseProgram(prg);
    //  });
    //});

    //it("should fail as a program is invalid", function () {
    //  testUtils.withContext(function (ctx) {
    //    var prg = cl.createProgramWithSource(ctx, squareKern + "$bad_inst");
    //    cl.linkProgram.bind(cl.linkProgram, ctx, null, null, 1, [prg])
    //      .should.throw(cl.INVALID_PROGRAM.message);
    //    cl.releaseProgram(prg);
    //  });
    //});
  });

  describe("#unloadPlatformCompiler", function () {
    it("should work when using a valid platform", function() {
      testUtils.withContext(function (ctx, device, platform) {
        cl.unloadPlatformCompiler(platform);
      })
    });
  });

  describe("#getProgramInfo", function () {
    var testForType = function(clKey, _assert) {
      it("should return the good type for " + clKey, function () {
        testUtils.withContext(function (ctx, device) {
          testUtils.withProgram(ctx, squareKern, function (prg) {

            var val = cl.getProgramInfo(prg, cl[clKey]);
            _assert(val);
            console.log(clKey + " = " + val);
          });
        })
      })
    };

    testForType("PROGRAM_REFERENCE_COUNT", assert.isNumber.bind(assert));
    testForType("PROGRAM_NUM_DEVICES", assert.isNumber.bind(assert));
    testForType("PROGRAM_CONTEXT", assert.isObject.bind(assert));
    testForType("PROGRAM_DEVICES", assert.isArray.bind(assert));
    testForType("PROGRAM_BINARY_SIZES", assert.isArray.bind(assert));
    testForType("PROGRAM_SOURCE", assert.isString.bind(assert));
    testForType("PROGRAM_KERNEL_NAMES", assert.isString.bind(assert));

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

    var testForType = function(clKey, _assert) {
      it("should return the good type for " + clKey, function () {
        testUtils.withContext(function (ctx, device) {
          testUtils.withProgram(ctx, squareKern, function (prg) {

            var val = cl.getProgramBuildInfo(prg, device, cl[clKey]);
            _assert(val);
            console.log(clKey + " = " + val);
          });
        })
      })
    };

    testForType("PROGRAM_BUILD_STATUS", assert.isNumber.bind(assert));
    testForType("PROGRAM_BUILD_OPTIONS", assert.isString.bind(assert))
    testForType("PROGRAM_BUILD_LOG", assert.isString.bind(assert))
    testForType("PROGRAM_BINARY_TYPE", assert.isNumber.bind(assert))

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
