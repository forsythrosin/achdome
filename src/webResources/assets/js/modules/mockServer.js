var callbacks = {};

function emit(eventType, err, data) {
  var cbs = callbacks[eventType];
  if (!cbs) return;
  for (var i = 0, len = cbs.length; i < len; i++) {
    cbs[i](err, data);
  }
};

var lc = 0, rc = 0,
    limit = 20;

function rand256() {
  return Math.floor(Math.random() * 256);
}

var server = {
  init: function () {
    emit('welcome');
  },
  register: function(name) {
    emit('lobby', null, {
      player: {
        name: name,
        color: [rand256(),rand256(),rand256()]
      }
    });
  },
  start: function () {
    emit('started');
  },
  left: function (down) {
    if (++lc + rc > limit) {
      emit('ended', null, {points: lc});
    }
  },
  right: function (down) {
    if (++rc + lc > limit) {
      emit('ended', null, {points: lc});
    }
  },
  on: function (eventType, callback) {
    if (!callbacks[eventType]) {
      callbacks[eventType] = [];
    }
    callbacks[eventType].push(callback);
  }
};

module.exports = server;
