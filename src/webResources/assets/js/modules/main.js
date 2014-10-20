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




$(function () {
  $(window).resize(function () {
    setTimeout(logotypeAnimator.resize, 0);
  });

  var $body = $('body');
  setButtonListeners($body);
  setServerListeners($body);

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
  var t = params.time * 1000 - 3000;
  if (t < 0) t = 0;

  logotypeAnimator.finishLoader();
  setTimeout(function () {
    renderNotMoving($container, params);
  }, t);
};

var renderNotMoving = function ($container, params) {
  $container.html(notMoving(params, options));
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

var me = {
  name: "",
  color: []
};

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
        if (res.name !== undefined) me.name = res.name;
        if (res.color !== undefined) me.color = res.color;
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
        var p = me;
        if (res.phi !== undefined && res.theta !== undefined) {
          var r = res.phi, t = res.theta;
          p.position = {
            x: (1 + r * Math.cos(t)) / 2 * 100 + '%',
            y: (1 + r * Math.sin(t)) / 2 * 100 + '%'
          }
        }
      }
      data.player = p;
      startCountdown($container, data);
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
        if (res.name !== undefined && res.color !== undefined) {
          data.killer = {
            name: res.name,
            color: res.color
          };
        }
        if (res.points !== undefined) {
          data.points = res.points;
        }
      }
      renderDied($container, data);
    }
  });
};
