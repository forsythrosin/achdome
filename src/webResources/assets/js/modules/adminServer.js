var server = require('./server');
var init = server.init;

var password;

var send = function(message, data) {
  data = data || {};
  data.password = password;
  server.sendToServer(message, data);
}

server.init = function() {
  init("admin");
};
server.authenticate = function (pwd) {
  password = pwd;
  send('authenticate_admin');
};
server.startGame = function() {
  send('start_game');
}
server.endGame = function() {
  send('end_game');
}

module.exports = server;
