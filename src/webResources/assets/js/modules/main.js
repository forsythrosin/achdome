var handlebars = require('handlebars'),
    server = require('./server'),
    logotypeAnimator = require('./logotypeAnimator'),
    $ = require('jquery');

// Handlebars
var register = require('../../templates/register.hbs'),
    registered = require('../../templates/registered.hbs'),
    notMoving = require('../../templates/notMoving.hbs'),
    moving = require('../../templates/moving.hbs'),
    died = require('../../templates/died.hbs'),
    disconnected = require('../../templates/disconnected.hbs');

// Partials
var _color = require('../../templates/partials/color.hbs'),
    _player = require('../../templates/partials/player.hbs'),
    _logotype = require('../../templates/partials/logotype.hbs');

var waitForCountdown = 2; // seconds


$(function () {
  var $body = $('body');
  setButtonListeners($body);
  setServerListeners($body);

  $(window).resize(function () {
    setTimeout(function() { resizeDome($body); }, 0);
    setTimeout(logotypeAnimator.resize, 0);
  });

  server.init();
});





// Helpers

var options = {
  partials: {
    color: _color,
    player: _player,
    logotype: _logotype
  }
};

var resizeDome = function($body) {
  $container = $body.find('#domeContainer');
  if ($container.length) {
    var mW, mH;
    var w = $container.width(),
        h = $container.height();
    if (w == h) {
      mW = 0;
      mH = 0;
    } else {
      var min = Math.min(w,h),
          max = Math.max(w,h),
          diff = max - min;
      if (w == min) {
        mW = 0;
        mH = diff/2;
      } else {
        mW = diff/2;
        mH = 0;
      }
    }
    $container.find('#dome').css({
      left: mW,
      right: mW,
      top: mH,
      bottom: mH
    });
  }
};

var renderDisconnected = function ($container, params) {
  $container.html(disconnected(params, options));
};

var renderRegister = function ($container, params) {
  $container.html(register(params, options));
  $form = $container.find('#registerForm').fadeTo(0, 0);
  $text = $container.find('#logotypeText').fadeTo(0, 0);
  logotypeAnimator.animate(1000, 100, 'easeInQuad', function () {
    $text.fadeTo(500, 1, function () {
      $form.fadeTo(500, 1);
    });
  });
};

var renderRegistered = function ($container, params) {
  $container.html(registered(params, options));
  logotypeAnimator.animateLoader(1000, 0);
};

var startCountdown = function ($container, params) {
  var wait = waitForCountdown;
  if (wait > params.time) wait = 0;
  params.time -= wait;

  logotypeAnimator.finishLoader();
  setTimeout(function () {
    renderNotMoving($container, params);
  }, wait * 1000);
};

var renderNotMoving = function ($container, params) {
  $container.html(notMoving(params, options));
  setTimeout(function() { resizeDome($container) }, 0);
};

var renderMoving = function ($container, params) {
  $container.html(moving(params, options));
};

var renderDied = function ($container, params) {
  $container.html(died(params, options));
};

var setButtonListeners = function ($container) {
  $container
    .on('click', '#play', function () {
      var $name = $container.find('input[name="name"]');
      var name = $name.val();
      if (name) {
        server.register(name);
        $name.removeClass('warning');
      } else {
        $name.addClass('warning').attr('placeholder', 'Choose a name');
      }
    })
    .on('click', '#positionInfo', function () {
      server.startMoving();
    })
    .on('touchstart', '#left', function () {
      $container.find('#left').addClass('active');
      server.left(true);
    })
    .on('touchend', '#left', function () {
      $container.find('#left').removeClass('active');
      server.left(false);
    })
    .on('touchstart', '#right', function () {
      $container.find('#right').addClass('active');
      server.right(true);
    })
    .on('touchend', '#right', function () {
      $container.find('#right').removeClass('active');
      server.right(false);
    });
};

var transformColor = function(color) {
  return [Math.floor(color[0] * 255), Math.floor(color[1] * 255), Math.floor(color[2] * 255)];
}

var me = {
  id: -1,
  name: "",
  color: []
};

var players = [];

var setServerListeners = function ($container) {
  server.on('connected', function (err, res) {
    if (err) console.warn(err);
    renderRegister($container, res);
  });

  server.on('disconnected', function(err, res) {
    if (err) console.warn(err);
    renderDisconnected($container, res);
  });

  server.on('registered', function (err, res) {
    if (err) {
      console.warn(err);
    } else {
      if (res !== undefined) {
        if (res.id !== undefined) me.id = res.id;
        if (res.name !== undefined) me.name = res.name;
        if (res.color !== undefined) me.color = transformColor(res.color);
      }
      renderRegistered($container, {player: me});
      $container.find('#logotypeText').show();
      logotypeAnimator.animateLoader(1000, 0);
    }
  });

  server.on('countdown', function (err, res) {
    if (err) {
      console.warn(err);
    } else {
      var data = {
        time: 0
      };
      if (res !== undefined) {
        if (res.time !== undefined) {
          data.time = res.time;
        }
        if (res.players !== undefined) {
          players = [];
          Object.keys(res.players).forEach(function(key) {
            var p = res.players[key];
            var pos = p.position;
            if (pos !== undefined && pos.phi !== undefined && pos.theta !== undefined) {
              var r = pos.theta / (Math.PI / 2), t = pos.phi;
              p.position = {
                x: (1 + r * Math.cos(t)) / 2 * 100 + '%',
                y: (1 - r * Math.sin(t)) / 2 * 100 + '%'
              }
            }
            if (p.color !== undefined) {
              p.color = transformColor(p.color);
            }
            if (key == me.id) {
              p.me = true;
              data.player = p;
            }
            p.id = key;
            players.push(p);
          });
          data.players = players;
          console.log('players', players);
        }
      }
      startCountdown($container, data);
    }
  });

  server.on('notMoving', function(err, res) {
    if (err) {
      console.warn(err);
    } else {
      renderNotMoving($container, {player: me, players: players});
    }
  });

  server.on('moving', function (err, res) {
    if (err) {
      console.warn(err);
    } else {
      renderMoving($container, {player: me});
    }
  });

  server.on('died', function (err, res) {
    if (err) {
      console.warn(err);
    } else {
      var data = {};
      if (res !== undefined) {
        if (res.killer !== undefined) {
          data.killer = res.killer;
          data.killer.color = transformColor(res.killer.color);
        }
        if (res.points !== undefined) {
          data.points = res.points;
        }
      }
      renderDied($container, data);
    }
  });
};
