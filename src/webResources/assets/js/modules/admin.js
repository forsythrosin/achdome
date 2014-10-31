var handlebars = require('handlebars'),
    server = require('./adminServer'),
    logotypeAnimator = require('./logotypeAnimator'),
    $ = require('jquery');

// Enable extension of jquery
window.jQuery = $;

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

var configSliders = {
  wormWidth: {
    range: [1, 10],
    value: null,
    scaleFactor: 200
  },
  wormSpeed: {
    range: [1, 10],
    value: null,
    scaleFactor: 500
  },
  countdown: {
    range: [0, 20],
    value: null,
    scaleFactor: 1
  }
};

var updateSettings = function ($container, data) {
  var settings = data.settings || {};
  Object.keys(settings).forEach(function (key) {
    var slide = configSliders[key];
    if (slide !== undefined) {
      var value = settings[key];
      slide.value = Math.round(slide.scaleFactor * value);
      $container.find('.slider#' + key).val(slide.value);
      console.log("Slider " + key + " set to " + value + " = " + slide.value);
    }
  });
};

var transformColor = function(color) {
  return [Math.floor(color[0] * 255), Math.floor(color[1] * 255), Math.floor(color[2] * 255), color[3]];
}

var updatePlayers = function ($container, data) {
  var players = data.players || {};
  $connectedPlayers = $container.find('#connectedPlayers');
  $players = $connectedPlayers.find('#players').empty();
  var numberOfPlayers = 0;
  Object.keys(players).forEach(function (key) {
    var player = players[key];
    player.color = transformColor(player.color);
    $players.append(_player(player, options));
    numberOfPlayers++;
  });
  $connectedPlayers.find('#playerCount').text(numberOfPlayers);
};

var renderAdminPanel = function ($container, params) {
  $container.html(adminPanel(params, options));
  Object.keys(configSliders).forEach(function (key) {
    var slide = configSliders[key];
    $container.find('.slider#' + key).noUiSlider({
      connect: "lower",
      start: slide.range[1],
      step: 1,
      range: {
        min: slide.range[0],
        max: slide.range[1]
      }
    }).Link('lower').to('-inline-<div class="tooltip"></div>', function (v) {
      var value = Math.floor(v);
      slide.value = value;
      $(this).html('<span>' + value + '</span>');
    });
  });
  updateSettings($container, params);
  updatePlayers($container, params);
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
    })
    .on('click', '#saveSettings', function() {
      var settings = {};
      Object.keys(configSliders).forEach(function (key) {
        var $slider = $container.find('.slider#' + key);
        if ($slider) {
          var slide = configSliders[key];
          var value = $slider.val();
          settings[key] = value / slide.scaleFactor;
        }
      });
      server.updateSettings(settings);
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
    .on('settingsChanged', function (err, res) {
      if (err) console.warn(err);
      updateSettings($container, res);
    })
    .on('playerListUpdated', function (err, res) {
      if (screen == "adminPanel") {
        updatePlayers($container, res);
      }
    })
    .on('gameStarted', function (err, res) {
      if (screen == "adminPanel") {
        $container.find('#changeGameState').removeClass().addClass('end').text("End game");
        $container.find('#state h1').text('Game started');
      } else {
        res = res || {};
        res.started = true;
        renderAdminPanel($container, res);
      }
    })
    .on('lobby', function (err, res) {
      if (screen == "adminPanel") {
        $container.find('#changeGameState').removeClass().addClass('start').text("Start game");
        $container.find('#state h1').text('In lobby');
      } else {
        res = res || {};
        res.ready = true;
        renderAdminPanel($container, res);
      }
    });
};