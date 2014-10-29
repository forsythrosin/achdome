var server = require('./server');
var init = server.init;

server.init = function() {
  init("player");
};
server.register = function (name) {
  server.sendToServer('register', {name: name});
};
server.unregister = function () {
  server.sendToServer('unregister');
};
server.startMoving = function () {
  server.sendToServer('start_moving');
};
server.left = function (down) {
  server.sendToServer('left_' + (down ? 'down' : 'up'));
};
server.right = function (down) {
  server.sendToServer('right_' + (down ? 'down' : 'up'));
};

module.exports = server;
