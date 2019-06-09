var cl = require('../lib/opencl');
var should = require('chai').should();
var util = require('util');
var log = console.log;
var assert = require("chai").assert;
var fs = require("fs");
var U = require("./utils/utils");
var skip = require("./utils/diagnostic");
var versions = require("./utils/versions");

var squareKern = fs.readFileSync(__dirname + "/kernels/square.cl").toString();
var squareCpyKern = fs.readFileSync(__dirname + "/kernels/square_cpy.cl").toString();

describe("Program", function () {

  describe("#createProgramWithSource", function () {

    it("should return a valid program", function () {
      U.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);

        assert.isNotNull(prg);
        assert.isDefined(prg);

        cl.releaseProgram(prg);
      });
    });

    it("should throw as context is invalid", function () {
      U.bind(cl.createProgramWithSource, null, squareKern)
        .should.throw(cl.INVALID_CONTEXT.message);
    });

  });

  describe("#buildProgram", function () {

    it("should build using a valid program and a given device", function () {
      U.withContext(function (ctx, device) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.buildProgram(prg, [device]);
        assert(ret == cl.SUCCESS);

        cl.releaseProgram(prg);
      });
    });

    it("should build using a valid program", function () {
      U.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.buildProgram(prg);
        assert(ret == cl.SUCCESS);

        cl.releaseProgram(prg);
      });
    });

    it("should build and call the callback using a valid program", function (done) {
      U.withAsyncContext(function (ctx,device,platform,ctxDone) {
        var mCB = function(userData, prg){
          assert.isNotNull(prg);
          assert.isDefined(prg);
          cl.releaseProgram(prg);
          ctxDone();
          userData.done();
        };
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.buildProgram(prg,undefined,undefined,mCB,{done:done});
        assert(ret == cl.SUCCESS);
      });
    });
    it("should build using a valid program and options", function () {
      U.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.buildProgram(prg, null, "-D NOCL_TEST=5");
        assert(ret == cl.SUCCESS);

        cl.releaseProgram(prg);
      });
    });


    it("should throw if program is NULL", function () {
      U.withContext(function (ctx) {
        U.bind(cl.buildProgram, null)
          .should.throw(cl.INVALID_PROGRAM.message);
      });
    });

    it("should throw if program is INVALID", function () {
      U.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern + "$bad_inst");
        U.bind(cl.buildProgram, prg)
          .should.throw(cl.BUILD_PROGRAM_FAILURE.message);
      });
    });

  });


  describe("#createProgramWithBinary", function () {

    it("should create a valid program from a binary", function () {

      U.withContext(function (ctx, device) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.buildProgram(prg, [device]);
        var bin = cl.getProgramInfo(prg, cl.PROGRAM_BINARIES);
        var sizes = cl.getProgramInfo(prg, cl.PROGRAM_BINARY_SIZES);
        //
        var prg2 = cl.createProgramWithBinary(ctx, [device], sizes, bin);

        assert.isNotNull(prg2);
        assert.isDefined(prg2);

        cl.releaseProgram(prg);
        cl.releaseProgram(prg2);
      });
    });

    skip().vendor('Intel').it("should create a valid program from a buffer", function () {

      U.withContext(function (ctx, device) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.buildProgram(prg, [device]);
        var bin = cl.getProgramInfo(prg, cl.PROGRAM_BINARIES).map(({ buffer }) => buffer);
        var sizes = cl.getProgramInfo(prg, cl.PROGRAM_BINARY_SIZES);
        //
        var prg2 = cl.createProgramWithBinary(ctx, [device], sizes, bin);

        assert.isNotNull(prg2);
        assert.isDefined(prg2);

        cl.releaseProgram(prg);
        cl.releaseProgram(prg2);
      });
    });

    it("should fail as binaries list is empty", function () {
      U.withContext(function (ctx, device) {
        U.bind(cl.createProgramWithBinary, ctx, [device], [], [])
          .should.throw(cl.INVALID_VALUE.message);
      });
    });


    it("should fail as lists are not of the same length", function () {

      U.withContext(function (ctx, device) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        cl.buildProgram(prg);
        var bin = cl.getProgramInfo(prg, cl.PROGRAM_BINARIES);
        var sizes = cl.getProgramInfo(prg, cl.PROGRAM_BINARY_SIZES);
        sizes.push(100);

        U.bind(cl.createProgramWithBinary, ctx, [device], sizes, bin)
        cl.releaseProgram(prg);
      });
    });

  });


  versions(["1.2","2.0"]).describe("#createProgramWithBuiltInKernels", function () {

    var f = cl.createProgramWithBuiltInKernels;

    it("should fail as context is invalid", function () {
      U.withContext(function (context, device) {
        U.bind(f, null, [device], ['a'])
          .should.throw(cl.INVALID_CONTEXT.message);
      })
    });

    it("should fail as device list is empty", function () {
      U.withContext(function (context, device) {
        U.bind(f, context, [], ['a'])
          .should.throw(cl.INVALID_VALUE.message);
      })
    });

    it("should fail as names list is empty", function () {
      U.withContext(function (context, device) {
        U.bind(f, context, [device], [])
          .should.throw(cl.INVALID_VALUE.message);
      })
    });

    it("should fail as names list contains non string values", function () {
      U.withContext(function (context, device) {
        U.bind(f, context, [device], [function(){}])
          .should.throw(cl.INVALID_VALUE.message);
      })
    });

    it("should fail as kernel name is unknown", function () {
      U.withContext(function (context, device) {
        U.bind(f, context, [device], ['nocl_test'])
          .should.throw(cl.INVALID_VALUE.message);
      })
    });
  });

  describe("#retainProgram", function () {
    it("should increment the reference count", function () {
      U.withContext(function (ctx) {
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
      U.withContext(function (ctx) {
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

  versions(["1.2", "2.0"]).describe("#compileProgram", function () {
    it("should build a program with no input headers", function () {
      U.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.compileProgram(prg);
        assert(ret == cl.SUCCESS);
        cl.releaseProgram(prg);
      });
    });

    it("should build and call the callback with no input header", function (done) {
      U.withAsyncContext(function (ctx,device,platform,ctxDone) {
        var mCB = function(userData, prg){
          assert.isNotNull(prg);
          assert.isDefined(prg);
          cl.releaseProgram(prg);
          ctxDone();
          userData.done();
        };
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.compileProgram(prg,undefined,undefined,undefined,undefined,mCB,{done:done});
        assert(ret == cl.SUCCESS);
      });
    });

    it("should build a program with an input header", function () {
      U.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var prg2 = cl.createProgramWithSource(ctx, squareKern);

        var ret = cl.compileProgram(prg, null, null, [prg2], ["prg2.h"]);
        assert(ret == cl.SUCCESS);
        cl.releaseProgram(prg);
      });
    });

    it("should fail as ain't no name for header", function () {
      U.withContext(function (ctx) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var prg2 = cl.createProgramWithSource(ctx, squareKern);

        U.bind(cl.compileProgram, prg, null, null, [prg2], [])
          .should.throw();

        cl.releaseProgram(prg);
        cl.releaseProgram(prg2);
      });
    });
  });

  versions(["1.2", "2.0"]).describe("#linkProgram", function () {


    it("should fail as context is invalid", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {

          U.bind(cl.linkProgram, null, null, null, [prg])
            .should.throw(cl.INVALID_CONTEXT.message);

        });
      });
    });

    it("should fail as program is of bad type", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {

          U.bind(cl.linkProgram,ctx, null, null, [ctx])
            .should.throw(cl.INVALID_PROGRAM.message);

        });
      });
    });

    skip().it("should fail as options sent to the linker are invalid", function () {
      U.withContext(function (ctx, device) {
        U.withProgram(ctx, squareKern, function (prg) {
          cl.compileProgram(prg);

          U.bind(cl.linkProgram,ctx, null, "-DnoCLtest=5", [prg])
            .should.throw(cl.INVALID_LINKER_OPTIONS.message);

        });
      });
    });

    it("should success in linking one compiled program", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          cl.compileProgram(prg);
          var nprg = cl.linkProgram(ctx, null, null, [prg]);

          assert.isObject(nprg);
        });
      });
    });
    
    it("should success in linking one program and call the callback", function (done) {
      U.withAsyncContext(function (ctx,device,platform,ctxDone) {
        var mCB = function(userData, ctx){
          assert.isNotNull(prg);
          assert.isDefined(prg);
          cl.releaseProgram(prg);
          ctxDone();
          userData.done();
        };
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var ret = cl.compileProgram(prg);
        assert(ret == cl.SUCCESS);
        var nprg = cl.linkProgram(ctx, null, null, [prg],mCB,{done:done});
        assert.isObject(nprg);

      });
    });

    it("should success in linking one compiled program with a list of devices", function () {
      U.withContext(function (ctx, device) {
        U.withProgram(ctx, squareKern, function (prg) {
          cl.compileProgram(prg);
          var nprg = cl.linkProgram(ctx, [device], null, [prg]);

          assert.isObject(nprg);
        });
      });
    });

    it("should success in linking two compiled programs", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          U.withProgram(ctx, squareCpyKern, function (prg2) {
            cl.compileProgram(prg);
            cl.compileProgram(prg2);

            var nprg = cl.linkProgram(ctx, null, null, [prg, prg2]);
            assert.isObject(nprg);
          });
        });
      });
    });
    });



  versions(["1.2", "2.0"]).describe("#unloadPlatformCompiler", function () {
    it("should work when using a valid platform", function() {
      U.withContext(function (ctx, device, platform) {
        cl.unloadPlatformCompiler(platform);
      })
    });
  });

  describe("#getProgramInfo", function () {
    var testForType = function(clKey, _assert) {
      it("should return the good type for " + clKey, function () {
        U.withContext(function (ctx, device) {
          U.withProgram(ctx, squareKern, function (prg) {

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
    if(cl.v12) {
      testForType("PROGRAM_KERNEL_NAMES", assert.isString.bind(assert));
    }

      it("should have the same program source as the one given", function () {
        U.withContext(function (ctx) {
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
        U.withContext(function (ctx, device) {
          U.withProgram(ctx, squareKern, function (prg) {

            var val = cl.getProgramBuildInfo(prg, device, cl[clKey]);
            _assert(val);
            console.log(clKey + " = " + val);
          });
        })
      })
    };

    testForType("PROGRAM_BUILD_STATUS", assert.isNumber.bind(assert));
    testForType("PROGRAM_BUILD_OPTIONS", assert.isString.bind(assert));
    testForType("PROGRAM_BUILD_LOG", assert.isString.bind(assert));
    if(cl.v12) {
      testForType("PROGRAM_BINARY_TYPE", assert.isNumber.bind(assert));
    }

    it("should return the same options string that was passed before", function () {
      U.withContext(function (ctx, device) {
        var prg = cl.createProgramWithSource(ctx, squareKern);
        var buildOpts = "-D NOCL_TEST=5";
        var ret = cl.buildProgram(prg, null, buildOpts);

        var opt = cl.getProgramBuildInfo(prg, device, cl.PROGRAM_BUILD_OPTIONS);
        assert(opt.indexOf(buildOpts) !== -1); // there is an extra space in get info output
        cl.releaseProgram(prg);
      });
    });


});
