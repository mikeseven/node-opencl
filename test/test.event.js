var cl=require('../lib/opencl'),
    should=require('chai').should(),
    assert = require("chai").assert,
    util=require('util'),
    log=console.log,
    Diag = require("./utils/diagnostic");
    U = require("./utils/utils.js");


var isValidCQ = function (cq) {
  assert.isNotNull(cq);
  assert.isDefined(cq);
};


describe("Event", function() {
  describe("#createUserEvent",function() {
    it("should create user Event",function(){
      U.withContext(function(ctx) {
        var uEvent = cl.createUserEvent(ctx);
        isValidCQ(uEvent);
        cl.releaseEvent(uEvent);
      })
    });
  });

  var platform = MAIN_PLATFORM_ID;

  var properties= [
    cl.CONTEXT_PLATFORM, platform
  ];
  var device=global.MAIN_DEVICE_ID;
  var ctx=cl.createContext(properties, [device], null, null);

  describe("#getEventInfo",function(){
    var uEvent = cl.createUserEvent(ctx);
    
    function testNumber(info,name,expected) {
      it("should equal "+info,function(){
        var result = cl.getEventInfo(uEvent,eval("cl."+name));
        assert.isNumber(result);
        assert.strictEqual(result,expected);
      });
    }
    function testObject(info,name) {
      it("should be able to retrieve "+info,function(){
        var result = cl.getEventInfo(uEvent,eval("cl."+name));
        isValidCQ(uEvent);
        assert.isObject(result);
      });
    }
    
    testNumber("event status to cl.SUBMITTED","EVENT_COMMAND_EXECUTION_STATUS",cl.SUBMITTED);
    testNumber("ref count to one","EVENT_REFERENCE_COUNT",1);
    testNumber("event type to UserEvent","EVENT_COMMAND_TYPE",cl.COMMAND_USER);
    testObject("the context","EVENT_CONTEXT");
    testObject("the command queue","EVENT_COMMAND_QUEUE");
    cl.releaseEvent(uEvent);


  });

  describe("#setUserEventStatus",function(){
    var uEvent;
    beforeEach(function(){
      uEvent = cl.createUserEvent(ctx);
    });
    afterEach(function(){
      cl.releaseEvent(uEvent);
    });
    it("should set the status to the good value",function(){
      cl.setUserEventStatus(uEvent,cl.COMPLETE);
      var result = cl.getEventInfo(uEvent,cl.EVENT_COMMAND_EXECUTION_STATUS);
      assert.strictEqual(result,cl.COMPLETE);
    });
    
    
    //bug in amd driver? 
    it("should throw an error for invalid value",function(){
      cl.setUserEventStatus.bind(cl.setUserEvent,uEvent,-1).should.throw(cl.INVALID_VALUE.message);
    });
    it("should throw an error because 2 change of the values for the same user event",function(){
      cl.setUserEventStatus(uEvent,cl.COMPLETE);
      cl.setUserEventStatus.bind(cl.setUserEvent,uEvent,cl.COMPLETE).should.throw(cl.INVALID_VALUE.message);
    });
  });
  describe("#Retain/Release",function(){
    var uEvent =cl.createUserEvent(ctx);
    it("Should increment and decrement refcount for event",function(){
      assert.strictEqual(cl.retainEvent(uEvent),cl.SUCCESS);
      var result = cl.getEventInfo(uEvent,cl.EVENT_REFERENCE_COUNT);
      assert.strictEqual(result,2);
      
      cl.releaseEvent(uEvent);
      result = cl.getEventInfo(uEvent,cl.EVENT_REFERENCE_COUNT);
      assert.strictEqual(result,1);

    });
    cl.releaseEvent(uEvent);
  });

  describe("#setEventCallback",function(){
      it("callback should be called",function(done){
        var myCallback = function(mEvent,status,userData){
          var mctx =cl.getEventInfo(mEvent,cl.EVENT_CONTEXT);
          cl.releaseEvent(mEvent);
          cl.releaseContext(mctx);
          userData.done();
        };
        var mctx=cl.createContext(properties, [device], null, null);
        var mEvent =cl.createUserEvent(mctx); 
        cl.setEventCallback(mEvent,cl.COMPLETE,myCallback,{done:done});
        cl.setUserEventStatus(mEvent,cl.COMPLETE);
      });
         
  });

  cl.releaseContext(ctx);
});
