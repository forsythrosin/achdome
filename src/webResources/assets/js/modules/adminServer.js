var server = require('./server');
var init = server.init;

server.init = function() {
  init("admin");
};
server.authenticate = function (password) {
  server.sendToServer('authenticate', {password: password});
};

module.exports = server;
