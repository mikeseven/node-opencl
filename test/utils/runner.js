var Mocha = require('mocha'),
  fs = require('fs'),
  path = require('path');
var Diag = require("./diagnostic");
var chalk = require("chalk");

require("./device_selection");
var mocha = new Mocha;

/*
fs.readdirSync(__dirname + "/..").filter(function(file){
  return file.substr(-3) === '.js';
}).forEach(function(file){
  mocha.addFile(
    path.join(__dirname + "/..", file)
  );
});
*/

mocha.addFile(
  path.join(__dirname + "/..", "test.queues.js")
);

// Now, you can run the tests.
mocha.run(function(failures){
  process.on('exit', function () {
    console.warn("Driver issues : " + Diag.exceptionsCount());
    var cnt = failures - Diag.exceptionsCount();
    if (cnt == 0) {
      console.log(chalk.green("This means that all tests failures are due to drivers"));
    } else {
      console.log(chalk.red("This means that " + cnt +
        " tests have really failed or failures are due to unreported driver issues"));
    }
    process.exit(cnt);
  });
});
