var cl = require('../lib/opencl'),
  should = require('chai').should(),
  util = require('util'),
  testUtils = require("../lib/test_utils"),
  log = console.log,
  assert = require("chai").assert,
  fs = require("fs");

var squareKern = fs.readFileSync(__dirname + "/kernels/square.cl").toString();
var squareCpyKern = fs.readFileSync(__dirname + "/kernels/square_cpy.cl").toString();


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

    // TODO

  });

  describe("#getKernelInfo", function () {

    var testForType = function(clKey, _assert) {
      it("should return the good type for " + clKey, function () {
        testUtils.withContext(function (ctx) {
          testUtils.withProgram(ctx, squareKern, function (prg) {
            var k = cl.createKernel(prg, "square");

            var val = cl.getKernelInfo(k, cl[clKey]);
            _assert(val);
            console.log(clKey + " = " + val);
          });
        })
      })
    };

    testForType("KERNEL_ATTRIBUTES", assert.isString.bind(assert));
    testForType("KERNEL_FUNCTION_NAME", assert.isString.bind(assert));
    testForType("KERNEL_REFERENCE_COUNT", assert.isNumber.bind(assert));
    testForType("KERNEL_NUM_ARGS", assert.isNumber.bind(assert));
    testForType("KERNEL_CONTEXT", assert.isObject.bind(assert));
    testForType("KERNEL_PROGRAM", assert.isObject.bind(assert));

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

          assert.isObject(c);
        });
      });
    });

    it("should return the corresponding program", function () {
      testUtils.withContext(function (ctx) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var p = cl.getKernelInfo(k, cl.KERNEL_PROGRAM);

          assert.isObject(p);
        });
      });
    });

  });

  describe("#getKernelArgInfo", function () {
    var testForType = function(clKey, _assert) {
      it("should return the good type for " + clKey, function () {
        testUtils.withContext(function (ctx) {
          testUtils.withProgram(ctx, squareKern, function (prg) {
            var k = cl.createKernel(prg, "square");

            var val = cl.getKernelArgInfo(k, 0, cl[clKey]);
            _assert(val);
            console.log(clKey + " = " + val);
          });
        })
      })
    };

    testForType("KERNEL_ARG_ADDRESS_QUALIFIER", assert.isNumber.bind(assert));
    testForType("KERNEL_ARG_ACCESS_QUALIFIER", assert.isNumber.bind(assert));
    testForType("KERNEL_ARG_TYPE_QUALIFIER", assert.isNumber.bind(assert));

    it("should return the corresponding names", function(){
      testUtils.withContext(function (ctx) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var n1 = cl.getKernelArgInfo(k, 0, cl.KERNEL_ARG_NAME);
          var n2 = cl.getKernelArgInfo(k, 1, cl.KERNEL_ARG_NAME);
          var n3 = cl.getKernelArgInfo(k, 2, cl.KERNEL_ARG_NAME);

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
          assert.equal(n3, "uint");
        });
      });
    });
  });

  describe("#getKernelWorkGroupInfo", function () {

    var testForType = function(clKey, _assert) {
      it("should return the good type for " + clKey, function () {
        testUtils.withContext(function (ctx, device) {
          testUtils.withProgram(ctx, squareKern, function (prg) {
            var k = cl.createKernel(prg, "square");

            var val = cl.getKernelWorkGroupInfo(k, device, cl[clKey]);
            _assert(val);
            console.log(clKey + " = " + val);
          });
        })
      })
    };

    testForType("KERNEL_COMPILE_WORK_GROUP_SIZE", assert.isArray.bind(assert));
    testForType("KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE", assert.isNumber.bind(assert));
    testForType("KERNEL_WORK_GROUP_SIZE", assert.isNumber.bind(assert));
    testForType("KERNEL_LOCAL_MEM_SIZE", assert.isNumber.bind(assert));
    testForType("KERNEL_PRIVATE_MEM_SIZE", assert.isNumber.bind(assert));


    it("should throw INVALID_VALUE when looking for KERNEL_GLOBAL_WORK_SIZE", function(){
      testUtils.withContext(function (ctx, device) {
        testUtils.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          cl.getKernelWorkGroupInfo.bind(cl.getKernelWorkGroupInfo,k, device, cl.KERNEL_GLOBAL_WORK_SIZE)
            .should.throw(cl.INVALID_VALUE.message);
        });
      });
    })
  });

});
