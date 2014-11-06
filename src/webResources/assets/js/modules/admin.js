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
    name: "Worm width",
    range: [1, 10],
    value: null,
    scaleFactor: 1000
  },
  wormSpeed: {
    name: "Worm speed",
    range: [1, 10],
    value: null,
    scaleFactor: 2500
  },
  turnSpeed: {
    name: "Turn speed",
    range: [1, 10],
    value: null,
    scaleFactor: 50
  },
  countdown: {
    name: "Countdown",
    range: [0, 20],
    value: null,
    scaleFactor: 1
  },
  timeBetweenGaps: {
    name: "Between gaps",
    range: [100, 800],
    step: 50,
    values: [null, null],
    scaleFactor: 1
  },
  timeInGap: {
    name: "In gap",
    range: [5, 40],
    step: 5,
    values: [null, null],
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
    } else {
      // Check if slider is range
      var prefix = key.substr(0,3),
          rangeType = key.substr(3);
      rangeType = rangeType.charAt(0).toLowerCase() + rangeType.slice(1);
      slide = configSliders[rangeType];
      if (slide !== undefined) {
        if (prefix == 'min') {
          var value = settings[key];
          slide.value = Math.round(slide.scaleFactor * value);
          $container.find('.slider#' + rangeType).val([slide.value,null]);
        } else if (prefix == 'max') {
          var value = settings[key];
          slide.value = Math.round(slide.scaleFactor * value);
          $container.find('.slider#' + rangeType).val([null,slide.value]);
        }
      }
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
    player.waiting = !player.inGame;
    $players.append(_player(player, options));
    numberOfPlayers++;
  });
  $connectedPlayers.find('#playerCount').text(numberOfPlayers);
};

var renderAdminPanel = function ($container, params) {
  $container.html(adminPanel(params, options));
  $params = $container.find('#configParams');
  Object.keys(configSliders).forEach(function (key) {
    var slide = configSliders[key];
    $slider = $('<div class="slider" id="' + key + '"></div>');
    $param = $('<div class="configParam">').append('<label>' + slide.name + '</label>').append($slider);
    var isRange = Array.isArray(slide.values);
    $slider.noUiSlider({
      connect: isRange ? true : "lower",
      start: isRange ? slide.range : slide.range[1],
      step: slide.step || 1,
      range: {
        min: slide.range[0],
        max: slide.range[1]
      }
    });
    if (isRange) {
      $slider.Link('lower').to('-inline-<div class="tooltip"></div>', function (v) {
        var value = Math.floor(v);
        slide.values[0] = value;
        $(this).html('<span>' + value + '</span>');
      }).Link('upper').to('-inline-<div class="tooltip"></div>', function (v) {
        var value = Math.floor(v);
        slide.values[1] = value;
        $(this).html('<span>' + value + '</span>');
      });
    } else {
      $slider.Link('lower').to('-inline-<div class="tooltip"></div>', function (v) {
        var value = Math.floor(v);
        slide.value = value;
        $(this).html('<span>' + value + '</span>');
      });
    }
    $params.append($param);
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
    .on('click', '#numberOfGames .down', function () {
      $numberOfGames = $container.find('#numberOfGames .value');
      var val = parseInt($numberOfGames.text());
      if (--val > 0)
        $numberOfGames.text(val);
    })
    .on('click', '#numberOfGames .up', function () {
      $numberOfGames = $container.find('#numberOfGames .value');
      var val = parseInt($numberOfGames.text());
      $numberOfGames.text(++val);
    })
    .on('click', '#startTournament', function () {
      $numberOfGames = $container.find('#numberOfGames .value');
      server.startTournament(parseInt($numberOfGames.text()));
    })
    .on('click', '#endTournament', function () {
      server.endTournament();
    })
    .on('click', '#endGame', function () {
      server.endGame();
    })
    .on('click', '#saveSettings', function() {
      var settings = {};
      Object.keys(configSliders).forEach(function (key) {
        var $slider = $container.find('.slider#' + key);
        if ($slider) {
          var slide = configSliders[key];
          if (Array.isArray(slide.values)) {
            var capitalized = key.charAt(0).toUpperCase() + key.slice(1);
            var values = $slider.val();
            settings['min' + capitalized] = values[0] / slide.scaleFactor;
            settings['max' + capitalized] = values[1] / slide.scaleFactor;
          } else {
            var value = $slider.val();
            settings[key] = value / slide.scaleFactor;
          }
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
      if (screen != "adminPanel") {
        renderAdminPanel($container, res);
      }
      $container.find('#startTournament, #numberOfGames').hide();
      $container.find('#endGame, #endTournament').show();
      $container.find('#state h1').text('Game started');
      updatePlayers($container, res);
    })
    .on('lobby', function (err, res) {
      if (screen != "adminPanel") {
        renderAdminPanel($container, res);
      }
      $container.find('#startTournament, #numberOfGames').show();
      $container.find('#endGame, #endTournament').hide();
      $container.find('#state h1').text('In lobby');
      updatePlayers($container, res);
    });
};
