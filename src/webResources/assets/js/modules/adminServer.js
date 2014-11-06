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
server.startTournament = function(numberOfGames) {
  send('start_tournament', {numberOfGames: numberOfGames});
};
server.endTournament = function() {
  send('end_tournament');
};
server.endGame = function() {
  send('end_game');
};
server.updateSettings = function(settings) {
  send('update_settings', settings);
}

module.exports = server;
