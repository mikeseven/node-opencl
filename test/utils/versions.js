var U = require("./utils");
var assert = require("chai").assert;

module.exports = function(lst){
  var concerned = lst.some(function(v){return U.checkVersion(v)});
  return {
    it : function () {
      if(!concerned) return;
      it.apply(it, arguments);
    },
    hasUndefined: function (f) {
      if(!concerned) return;
      it("should be undefined as function does not exist in this version of OpenCL", function () {
        assert.isUndefined(f);
      });
    }
  }
};
