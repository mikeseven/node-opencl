var cl = require('../lib/opencl');
var should = require('chai').should();
var assert = require('chai').assert;
var util = require('util');
var U = require("./utils/utils");
var log = console.log;
var versions = require("./utils/versions");
var Diag = require("./utils/diagnostic");

describe("Pipes", function () {

  var platforms = cl.getPlatformIDs();
  var platform = platforms[0];
  var devices = cl.getDeviceIDs(platform);

  var device = devices[global.MAIN_DEVICE_IDX];

  versions(["1.x"]).hasUndefined(cl.createPipe);
  versions(["1.x"]).hasUndefined(cl.getPipeInfo);

  versions(["2.x"]).describe("#createPipe", function () {

    it("should succeed in creating a pipe", function () {
      U.withContext(function (ctx) {
        var pipe = cl.createPipe(ctx, 0, 8, 100, null);

        assert.isObject(pipe);
      });
    });

    it("should fail as packets is of size 0", function () {
      U.withContext(function (ctx) {
        U.bind(cl.createPipe, ctx, 0, 0, 100, null)
          .should.throw(cl.INVALID_PIPE_SIZE.message);
      });
    });

    it("should fail as packets max == 0", function () {
      U.withContext(function (ctx) {
        U.bind(cl.createPipe, ctx, 0, 8, 0, null)
          .should.throw(cl.INVALID_PIPE_SIZE.message);
      });
    });

    it("should fail as properties is not null", function () {
      U.withContext(function (ctx) {
        U.bind(cl.createPipe, ctx, 0, 8, 100, ["a"])
          .should.throw(cl.INVALID_VALUE.message);
      });
    });

    it("should fail as context is invalid", function () {
      U.withContext(function (ctx) {
        U.bind(cl.createPipe, null, 0, 8, 100, null)
          .should.throw(cl.INVALID_CONTEXT.message);
      });
    });

  });


  versions(["2.x"]).describe("#getPipeInfo", function () {

    var testForType = function (clKey, _assert) {
      it("should return the good type for " + clKey, function () {
        U.withContext(function (ctx) {
          var pipe = cl.createPipe(ctx, 0, 8, 100, null);
          var val = cl.getPipeInfo(pipe, cl[clKey]);
          _assert(val);
          console.log(clKey + " = " + val);
        })
      })
    };

    testForType("PIPE_PACKET_SIZE", assert.isNumber.bind(assert));
    testForType("PIPE_MAX_PACKETS", assert.isNumber.bind(assert));

    it("should return the same size of packets", function () {
      U.withContext(function (ctx) {

        var pipe = cl.createPipe(ctx, 0, 8, 100, null);
        var size = cl.getPipeInfo(pipe, cl.PIPE_PACKET_SIZE);

        assert(size === 8);
      });
    });

    it("should return the same packets max", function () {
      U.withContext(function (ctx) {

        var pipe = cl.createPipe(ctx, 0, 8, 100, null);
        var num = cl.getPipeInfo(pipe, cl.PIPE_MAX_PACKETS);

        assert(num === 100);
      });
    });


  });

});
