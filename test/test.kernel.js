var cl = require('../lib/opencl');
var should = require('chai').should();
var util = require('util');
var U = require("./utils/utils");
var log = console.log;
var assert = require("chai").assert;
var fs = require("fs");

var squareKern = fs.readFileSync(__dirname + "/kernels/square.cl").toString();
var squareCpyKern = fs.readFileSync(__dirname + "/kernels/square_cpy.cl").toString();

describe("Kernel", function () {

  describe("#createKernel", function () {

    it("should return a valid kernel", function () {
      U.withContext(function(ctx){
        U.withProgram(ctx, squareKern, function(prg){
          var k = cl.createKernel(prg, "square");

          assert.isNotNull(k);
          assert.isDefined(k);
          cl.releaseKernel(k);
        });
      });
    });

    it("should fail as kernel does not exists", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          U.bind(cl.createKernel, prg, "i_do_not_exist").should.throw();
        });
      });
    });

  });


  describe("#createKernelsInProgram", function () {

    it("should return two valid kernels", function() {
      U.withContext(function (ctx) {
        U.withProgram(ctx, [squareKern, squareCpyKern].join("\n"), function (prg) {
          var kerns = cl.createKernelsInProgram(prg);

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
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");

          var before = cl.getKernelInfo(k, cl.KERNEL_REFERENCE_COUNT);
          cl.retainKernel(k);
          var after = cl.getKernelInfo(k, cl.KERNEL_REFERENCE_COUNT);
          assert(before + 1 == after);
          cl.releaseKernel(k);
          cl.releaseKernel(k);
        });
      });
    });

  });

  describe("#releaseKernel", function () {

    it("should decrement reference count", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
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

    it("should successfully accept a memobject as first argument", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var mem = cl.createBuffer(ctx, 0, 8, null);

          if (cl.v12) {
            assert.equal(cl.setKernelArg(k, 0, null, mem), cl.SUCCESS, `setKernelArg should succeed`);
          }
          assert.equal(cl.setKernelArg(k, 0, "float*", mem), cl.SUCCESS, `setKernelArg should succeed`);

          cl.releaseMemObject(mem);
          cl.releaseKernel(k);
        });
      });
    });

    it("should fail when passed a scalar type as first argument (expected : memobject)", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");

          if (cl.v12) {
            U.bind(cl.setKernelArg, k, 0, null, 5)
              .should.throw(cl.INVALID_MEM_OBJECT.message);
          }
          U.bind(cl.setKernelArg, k, 0, "float*", 5)
          .should.throw(cl.INVALID_MEM_OBJECT.message);

          cl.releaseKernel(k);
        });
      });
    });

    it("should fail when passed a vector type as first argument (expected : memobject)", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");

          if (cl.v12) {
            U.bind(cl.setKernelArg, k, 0, null, [5, 10, 15])
              .should.throw(cl.INVALID_MEM_OBJECT.message);
          }
          U.bind(cl.setKernelArg, k, 0, "float*", [5, 10, 15])
          .should.throw(cl.INVALID_MEM_OBJECT.message);

          cl.releaseKernel(k);
        });
      });
    });

    it("should successfully accept an integer as third argument", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");

          if (cl.v12) {
            assert(cl.setKernelArg(k, 2, null, 5) == cl.SUCCESS);
          }
          assert(cl.setKernelArg(k, 2, "uint", 5) == cl.SUCCESS);

          cl.releaseKernel(k);
        });
      });
    });

    it("should fail when passed a char as third argument (expected : integer)", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");

          if (cl.v12) {
            U.bind(cl.setKernelArg, k, 2, null, "a")
              .should.throw(cl.INVALID_ARG_VALUE.message);
          }
          U.bind(cl.setKernelArg, k, 2, "char", "a")
            .should.throw(cl.INVALID_ARG_VALUE.message);

          cl.releaseKernel(k);
        });
      });
    });

    it("should fail when passed a vector as third argument (expected : integer)", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");

          if (cl.v12) {
            U.bind(cl.setKernelArg, k, 2, null, [5, 10, 15])
              .should.throw(cl.INVALID_ARG_VALUE.message);
          }
          U.bind(cl.setKernelArg, k, 2, "int", [5, 10, 15])
            .should.throw(cl.INVALID_ARG_VALUE.message);

          cl.releaseKernel(k);
        });
      });
    });


    it("should fail when passed a memobject as third argument (expected : integer)", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var mem = cl.createBuffer(ctx, 0, 8, null);

          if (cl.v12) {
            U.bind(cl.setKernelArg, k, 2, null, mem)
              .should.throw(cl.INVALID_ARG_VALUE.message);
          }
          U.bind(cl.setKernelArg, k, 2, "int", mem)
          .should.throw(cl.INVALID_ARG_VALUE.message);

          cl.releaseKernel(k);
        });
      });
    });



    it("should fail when passed a fourth argument on a kernel that only have three", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");

          if (cl.v12) {
            U.bind(cl.setKernelArg, k, 3, null, 5)
              .should.throw(cl.INVALID_VALUE.message);
          }
          U.bind(cl.setKernelArg, k, 3, "int", 5)
            .should.throw(cl.INVALID_ARG_INDEX.message);

          cl.releaseKernel(k);
        });
      });
    });

  });

  describe("#getKernelInfo", function () {

    var testForType = function(clKey, _assert) {
      it("should return the good type for " + clKey, function () {
        U.withContext(function (ctx) {
          U.withProgram(ctx, squareKern, function (prg) {
            var k = cl.createKernel(prg, "square");
            var val = cl.getKernelInfo(k, cl[clKey]);
            cl.releaseKernel(k);
            _assert(val);
            console.log(clKey + " = " + val);
          });
        })
      })
    };
    if(cl.v12){
      testForType("KERNEL_ATTRIBUTES", assert.isString.bind(assert));
    }
    testForType("KERNEL_FUNCTION_NAME", assert.isString.bind(assert));
    testForType("KERNEL_REFERENCE_COUNT", assert.isNumber.bind(assert));
    testForType("KERNEL_NUM_ARGS", assert.isNumber.bind(assert));
    testForType("KERNEL_CONTEXT", assert.isObject.bind(assert));
    testForType("KERNEL_PROGRAM", assert.isObject.bind(assert));

    it("should return the corresponding number of arguments", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var nb_args = cl.getKernelInfo(k, cl.KERNEL_NUM_ARGS);
          cl.releaseKernel(k);
          if (nb_args != 3) {
            assert.fail(nb_args, 3);
          }
        });
      });
    });

    it("should return the corresponding kernel name", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var name = cl.getKernelInfo(k, cl.KERNEL_FUNCTION_NAME);
          cl.releaseKernel(k);
          if (name != "square") {
            assert.fail(name, "square");
          }
        });
      });
    });

    it.skip("should return the corresponding context", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var c = cl.getKernelInfo(k, cl.KERNEL_CONTEXT);
          cl.releaseKernel(k);
          assert(c === ctx, 'c === ctx');
        });
      });
    });

    it.skip("should return the corresponding program", function () {
      U.withContext(function (ctx) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          var p = cl.getKernelInfo(k, cl.KERNEL_PROGRAM);
          cl.releaseKernel(k);
          assert(p === prg, 'p === prg');
        });
      });
    });

  });

  describe("#getKernelArgInfo", function () {
    var testForType = function(clKey, _assert) {
      it("should return the good type for " + clKey, function () {
        U.withContext(function (ctx) {
          U.withProgram(ctx, squareKern, function (prg) {
            var k = cl.createKernel(prg, "square");
            var val = cl.getKernelArgInfo(k, 0, cl[clKey]);
            cl.releaseKernel(k);
            _assert(val);
            console.log(clKey + " = " + val);
          });
        })
      })
    };
    if(cl.v20) {
      testForType("KERNEL_ARG_ADDRESS_QUALIFIER", assert.isNumber.bind(assert));
      testForType("KERNEL_ARG_ACCESS_QUALIFIER", assert.isNumber.bind(assert));
      testForType("KERNEL_ARG_TYPE_QUALIFIER", assert.isNumber.bind(assert));
    }

    if(cl.v20){
      it("should return the corresponding names", function(){
        U.withContext(function (ctx) {
          U.withProgram(ctx, squareKern, function (prg) {
            var k = cl.createKernel(prg, "square");
            var n1 = cl.getKernelArgInfo(k, 0, cl.KERNEL_ARG_NAME);
            var n2 = cl.getKernelArgInfo(k, 1, cl.KERNEL_ARG_NAME);
            var n3 = cl.getKernelArgInfo(k, 2, cl.KERNEL_ARG_NAME);
            cl.releaseKernel(k);
            assert.equal(n1, "input");
            assert.equal(n2, "output");
            assert.equal(n3, "count");
          });
        });
      });
    
      it("should return the corresponding types", function(){
        U.withContext(function (ctx) {
          U.withProgram(ctx, squareKern, function (prg) {
            var k = cl.createKernel(prg, "square");
            var n1 = cl.getKernelArgInfo(k, 0, cl.KERNEL_ARG_TYPE_NAME);
            var n2 = cl.getKernelArgInfo(k, 1, cl.KERNEL_ARG_TYPE_NAME);
            var n3 = cl.getKernelArgInfo(k, 2, cl.KERNEL_ARG_TYPE_NAME);
            cl.releaseKernel(k);
            assert.equal(n1, "float*");
            assert.equal(n2, "float*");
            assert.equal(n3, "uint");
          });
        });
      });
    }
  });
  describe("#getKernelWorkGroupInfo", function () {

    var testForType = function(clKey, _assert) {
      it("should return the good type for " + clKey, function () {
        U.withContext(function (ctx, device) {
          U.withProgram(ctx, squareKern, function (prg) {
            var k = cl.createKernel(prg, "square");
            var val = cl.getKernelWorkGroupInfo(k, device, cl[clKey]);
            cl.releaseKernel(k);
            _assert(val);
            console.log(clKey + " = " + val);
          });
        })
      })
    };

    testForType("KERNEL_COMPILE_WORK_GROUP_SIZE", assert.isArray.bind(assert));
    testForType("KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE", assert.isNumber.bind(assert));
    testForType("KERNEL_WORK_GROUP_SIZE", assert.isNumber.bind(assert));
    testForType("KERNEL_LOCAL_MEM_SIZE", assert.isArray.bind(assert));
    testForType("KERNEL_PRIVATE_MEM_SIZE", assert.isArray.bind(assert));


    it("should throw INVALID_VALUE when looking for KERNEL_GLOBAL_WORK_SIZE", function(){
      U.withContext(function (ctx, device) {
        U.withProgram(ctx, squareKern, function (prg) {
          var k = cl.createKernel(prg, "square");
          cl.getKernelWorkGroupInfo.bind(cl.getKernelWorkGroupInfo,k, device, cl.KERNEL_GLOBAL_WORK_SIZE)
            .should.throw(cl.INVALID_VALUE.message);
          cl.releaseKernel(k);
        });
      });
    })
  });
});
