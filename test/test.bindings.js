var cl = require('bindings')('opencl.node');
var should = require('chai').should();
var assert = require('chai').assert;
var util = require('util');
var U = require("./utils/utils");

describe("Equality", function() {

  it("should be falsy when trying to compare a NoCLObject with a scalar", function () {
    U.withContext(function (ctx) {
      console.log(util.inspect(cl, false, null))
      // TODO there is no ctx.equals()
      // assert.equal(ctx.equals(5), false, "should be false");
    });
  });


  // it("should be falsy when trying to compare a NoCLObject with a different one", function () {
  //   U.withContext(function (ctx, device) {
  //     U.withCQ(ctx, device, function (cq) {
  //       // TODO there is no ctx.equals()
  //       // assert.equal(ctx.equals(cq), false, "should be false");
  //     });
  //   });
  // });

  // it("should be truthy when comparing a NoCLObject with itself", function () {
  //   U.withContext(function (ctx) {
  //     // TODO there is no ctx.equals()
  //     // assert.equal(ctx.equals(ctx), true, "should be true")
  //   });
  // });

});
