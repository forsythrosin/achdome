var handlebars = require('handlebars'),
    server = require('./adminServer'),
    logotypeAnimator = require('./logotypeAnimator'),
    $ = require('jquery');

// Handlebars
var authenticateAdmin = require('../../templates/authenticateAdmin.hbs'),
    disconnected = require('../../templates/disconnected.hbs'),
    adminPanel = require('../../templates/adminPanel.hbs');

// Partials
var _color = require('../../templates/partials/color.hbs'),
    _player = require('../../templates/partials/player.hbs'),
    _identifier = require('../../templates/partials/identifier.hbs'),
    _logotype = require('../../templates/partials/logotype.hbs');

$(function () {
  var $body = $('body');
  setButtonListeners($body);
  setServerListeners($body);

  $(window).resize(function () {
    setTimeout(logotypeAnimator.resize, 0);
  });

  server.init();
});

// Helpers

var options = {
  partials: {
    color: _color,
    player: _player,
    identifier: _identifier,
    logotype: _logotype,
  }
};

var screen = "";

var renderDisconnected = function ($container, params) {
  $container.html(disconnected(params, options));
  screen = "disconnected";
};

var renderAuthenticate = function ($container, params) {
  $container.html(authenticateAdmin(params, options));
  $form = $container.find('#registerForm').fadeTo(0, 0);
  $text = $container.find('#logotypeText').fadeTo(0, 0);
  logotypeAnimator.animate(1000, 100, 'easeInQuad', function () {
    $text.fadeTo(500, 1, function () {
      $form.fadeTo(500, 1);
    });
  });
  screen = "authenticate";
};

var renderAdminPanel = function ($container, params) {
  $container.html(adminPanel(params, options));
  screen = "adminPanel";
};

var setButtonListeners = function ($container) {
  $container
    .on('click', '#authenticate', function () {
      var $password = $container.find('input[name="password"]');
      var password = $password.val();
      if (password) {
        server.authenticate(password);
        $password.removeClass('warning');
      } else {
        $password.addClass('warning').attr('placeholder', 'Enter password');
      }
    })
    .on('click', '#changeGameState.start', function () {
      server.startGame();
    })
    .on('click', '#changeGameState.end', function () {
      server.endGame();
    });
};

var setServerListeners = function ($container) {
  server
    .on('disconnected', function(err, res) {
      if (err) console.warn(err);
      renderDisconnected($container, res);
    })
    .on('connected authenticate', function (err, res) {
      if (err) console.warn(err);
      renderAuthenticate($container, res);
    })
    .on('refused', function(err, res) {
      if (err) console.warn(err);
      if (screen == "authenticate") {
        var $password = $container.find('input[name="password"]');
        $password.addClass('warning').val("").attr('placeholder', 'Wrong password');
      } else {
        renderAuthenticate($container, res);
      }
    })
    .on('authenticated', function (err, res) {
      if (err) console.warn(err);
      renderAdminPanel($container, res);
    })
    .on('gameStarted', function (err, res) {
      if (screen == "adminPanel") {
        $('#changeGameState').removeClass().addClass('end').text("End game");
      } else {
        res = res || {};
        res.started = true;
        renderAdminPanel($container, res);
      }
    })
    .on('lobby', function (err, res) {
      if (screen == "adminPanel") {
        $('#changeGameState').removeClass().addClass('start').text("Start game");
      } else {
        res = res || {};
        res.ready = true;
        renderAdminPanel($container, res);
      }
    });
};
