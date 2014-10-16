var handlebars = require('handlebars');
var server = require('./server');
var $ = require('jquery');

var welcomeScreen = require('../../templates/welcome.hbs');
var lobby = require('../../templates/lobby.hbs');
var startedScreen = require('../../templates/started.hbs');
var resultsScreen = require('../../templates/results.hbs');
var options = {
  partials: {
    player: require('../../templates/partials/player.hbs')
  }
};

$(function() {
  var $body = $('body');

  $body
    .on('click', '#play', function() {
      var $name = $body.find('input[name="name"]');
      var name = $name.val();
      if (name) {
        server.register(name);
        $name.removeClass('warning');
      } else {
        $name.addClass('warning').attr('placeholder', 'Choose a name');
      }
    })
    .on('click', '#start', function() {
      server.start();
    })
    .on('touchstart', '#left', function() {
      $body.find('#left').addClass('active');
      server.left(true);
    })
    .on('touchend', '#left', function() {
      $body.find('#left').removeClass('active');
      server.left(false);
    })
    .on('touchstart', '#right', function() {
      $body.find('#right').addClass('active');
      server.right(true);
    })
    .on('touchend', '#right', function() {
      $body.find('#right').removeClass('active');
      server.right(false);
    });

  $body.html(welcomeScreen({/*waiting: true*/}));

  server.on('welcome', function(err, data) {
    console.log('Welcome');
    var d = {};
    if (err) {
      d.error = err;
    }
    $body.html(welcomeScreen(d));
  });

  server.on('lobby', function (err, data) {
    console.log('Lobby', err, data);
    data.player.me = true;
    $body.html(lobby(data, options));
  });

  server.on('started', function (err, res) {
    console.log('Started');
    $body.html(startedScreen());
  });

  server.on('ended', function (err, res) {
    console.log('Ended');
    $body.html(resultsScreen({points: res.points}));
  });

  server.init();
});
