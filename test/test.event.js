var cl=require('../lib/opencl'),
    should=require('chai').should(),
    assert = require("chai").assert,
    util=require('util'),
    log=console.log,
    skip = require("./utils/diagnostic");
    U = require("./utils/utils.js");


var isValid = function (e) {
  assert.isNotNull(e);
  assert.isDefined(e);
};


describe("Event", function() {

  describe("#createUserEvent",function() {
    it("should create user Event",function(){
      U.withContext(function(ctx) {
        var uEvent = cl.createUserEvent(ctx);
        isValid(uEvent);
        cl.releaseEvent(uEvent);
      })
    });
  });

  describe("#getEventInfo",function(){
    function testNumber(info,name,expected) {
      it("should return the good value for " + name, function () {
        U.withContext(function (ctx) {
          var uEvent = cl.createUserEvent(ctx);
          var val = cl.getEventInfo(uEvent, cl[name]);
          assert.isNumber(val);
          assert.strictEqual(expected, val);
          console.log(name + " = " + val);
          cl.releaseEvent(uEvent);
        });
      });
    }

    function testObject(info,name) {
      it("should return the good value for " + name, function () {
        U.withContext(function (ctx) {
          var uEvent = cl.createUserEvent(ctx);
          var val = cl.getEventInfo(uEvent, cl[name]);
          assert.isObject(val);
          console.log(name + " = " + val);
          cl.releaseEvent(uEvent);
        })
      });
    }
    
    testNumber("event status to cl.SUBMITTED","EVENT_COMMAND_EXECUTION_STATUS",cl.SUBMITTED);

    skip().vendor("AMD").it("should return the good value for EVENT_REFERENCE_COUNT", function () {
      U.withContext(function (ctx) {
        var uEvent = cl.createUserEvent(ctx);
        var val = cl.getEventInfo(uEvent, cl.EVENT_REFERENCE_COUNT);
        assert.isNumber(val);
        assert.strictEqual(1, val);
        console.log(name + " = " + val);
        cl.releaseEvent(uEvent);
      });
    });

    testNumber("event type to UserEvent","EVENT_COMMAND_TYPE",cl.COMMAND_USER);
    testObject("the context","EVENT_CONTEXT");
    testObject("the command queue","EVENT_COMMAND_QUEUE");

  });

  describe("#setUserEventStatus",function(){

    it("should set the status to the good value",function(){
      U.withContext(function (ctx) {
        var uEvent = cl.createUserEvent(ctx);
        cl.setUserEventStatus(uEvent,cl.COMPLETE);
        var result = cl.getEventInfo(uEvent,cl.EVENT_COMMAND_EXECUTION_STATUS);
        assert.strictEqual(result,cl.COMPLETE);
        cl.releaseEvent(uEvent);
      });
    });
    
    //bug in amd driver? 
    skip().it("should throw an error for invalid value",function(){
      U.withContext(function (ctx) {
        var uEvent = cl.createUserEvent(ctx);
        cl.setUserEventStatus.bind(cl.setUserEvent,uEvent,-1).should.throw(cl.INVALID_VALUE.message);
        cl.releaseEvent(uEvent);
      });
    });

    skip().it("should throw an error because 2 change of the values for the same user event",function(){
      U.withContext(function (ctx) {
        var uEvent = cl.createUserEvent(ctx);
        cl.setUserEventStatus(uEvent,cl.COMPLETE);
        cl.setUserEventStatus.bind(cl.setUserEvent,uEvent,cl.COMPLETE).should.throw(cl.INVALID_VALUE.message);
        cl.releaseEvent(uEvent);
      });
    });
  });

  describe("#retainEvent", function() {

    it("should have incremented ref count after call", function () {
      U.withContext(function (ctx, device) {
        var uEvent = cl.createUserEvent(ctx);
        var before = cl.getEventInfo(uEvent, cl.EVENT_REFERENCE_COUNT);
        cl.retainEvent(uEvent);
        var after = cl.getEventInfo(uEvent, cl.EVENT_REFERENCE_COUNT);
        assert(before + 1 == after);
      });
    });
  });

  describe("#releaseEvent", function() {

    it("should have decremented ref count after call", function () {
      U.withContext(function (ctx, device) {
        var uEvent = cl.createUserEvent(ctx);
        var before = cl.getEventInfo(uEvent, cl.EVENT_REFERENCE_COUNT);
        cl.retainEvent(uEvent);
        cl.releaseEvent(uEvent);
        var after = cl.getEventInfo(uEvent, cl.EVENT_REFERENCE_COUNT);
        assert(before == after);
      });
    });

  });

  describe("#setEventCallback",function(){
      it("callback should be called",function(done){
        U.withAsyncContext(function (ctx, device, platform, ctxDone) {
          var myCallback = function(mEvent,status,userData){
            var mctx =cl.getEventInfo(mEvent,cl.EVENT_CONTEXT);
            cl.releaseEvent(mEvent);
            userData.done();
            ctxDone();
          };
          var mEvent =cl.createUserEvent(ctx);
          cl.setEventCallback(mEvent,cl.COMPLETE,myCallback,{done:done});
          cl.setUserEventStatus(mEvent,cl.COMPLETE);
        })
      });
         
  });

});
