var handlebars = require('handlebars');
var server = require('./server');
var $ = require('jquery');

var welcomeScreen = require('../../templates/welcome.hbs');
var lobby = require('../../templates/lobby.hbs');
var startedScreen = require('../../templates/started.hbs');
var resultsScreen = require('../../templates/results.hbs');
var options = {
  partials: {
    color: require('../../templates/partials/color.hbs'),
    player: require('../../templates/partials/player.hbs'),
    logotype: require('../../templates/partials/logotype.hbs')
  }
};

$(function() {
  var $body = $('body');
  var me = {
    name: "",
    color: []
  };

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
    .on('click', '#positionInfo', function() {
      server.startMoving();
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

  var logotypeSettings = {
    size: 256,
    delay: 500,
    length: 1000,
    ease: 'easeInQuad'
  };

  $body.html(welcomeScreen({logotypeSettings: logotypeSettings}, options));

  server.on('welcome', function(err, data) {
    console.log('Welcome', err, data);
    var d = {
      logotypeSettings: logotypeSettings
    };
    if (err) {
      d.error = err;
    }
    $body.html(welcomeScreen(d, options));
  });

  server.on('connected', function(err, res) {
    console.log('Connected', err, res);
    if (res !== undefined && res.player !== undefined) {
      var player = res.player;
      if (player.name !== undefined) {
        me.name = player.name;
      }
      if (player.color !== undefined) {
        me.color = player.color;
      }
    }
    var data = {
      player: me
    };
    console.log('Rendering lobby with ', data);
    $body.html(lobby(data, options));
  });

  server.on('lobby', function (err, res) {
    console.log('Lobby', err, res);
    var data = {
      player: me
    };
    if (res !== undefined && res.results !== undefined) {
      data.results = res.results;
    }
    $body.html(lobby(data, options));
  });

  server.on('game_started', function (err, res) {
    console.log('Game started', err, res);
    // pass through the players position on the screen
    $body.html(startedScreen({player: me, position: {phi:3.14/4,theta:3.14/4}}, options));
  });

  server.on('started_moving', function(err, res) {
    console.log('Started moving', err, res);
    $body.html(startedScreen({player: me}, options));
  });

  server.on('killed', function (err, res) {
    console.log('Killed', err, res);
    $body.html(resultsScreen({points: res.points}, options));
  });

  $(window).resize(function() {

  });

  server.init();
});
