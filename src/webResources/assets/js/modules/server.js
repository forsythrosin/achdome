var callbacks = {},
    socket,
    connected;

function get_appropriate_ws_url() {
  var pcol;
  var u = document.URL;

  /*
   * We open the websocket encrypted if this page came on an
   * https:// url itself, otherwise unencrypted
   */

  if (u.substring(0, 5) == "https") {
    pcol = "wss://";
    u = u.substr(8);
  } else {
    pcol = "ws://";
    if (u.substring(0, 4) == "http")
      u = u.substr(7);
  }

  u = u.split('/');

  /* + "/xxx" bit is for IE10 workaround */

  return pcol + u[0] + "/xxx";
}

function setupSocket() {
  function onopen() {
    connected = true;
    emit('connected', null, null);
  }

  function onclose() {
    console.log('Server disconnected, reconnecting...');
    connected = false;
    emit('disconnected', null, null);
    setTimeout(function () {
      // Connection has closed so try to reconnect every 1 second.
      createSocket();
    },  1000);
  }

  function onmessage(event) {
    console.log('Message received', event);
    var e = JSON.parse(event.data);
    emit(e.message, null, e.data);
  }

  function createSocket() {
    console.log('Connecting to server');
    if (typeof MozWebSocket != "undefined") {
      socket = new MozWebSocket(get_appropriate_ws_url(), "sgct");
    } else {
      socket = new WebSocket(get_appropriate_ws_url(), "sgct");
    }
    socket.onopen = onopen;
    socket.onclose = onclose;
    socket.onmessage = onmessage;
  }

  createSocket();
}

function sendToServer(message, data) {
  console.log('Sending', message, data);
  if (connected) {
    try {
      socket.send(JSON.stringify({"message": message, "data": data}));
      return true;
    } catch(exception) {
      console.warn('Exception when sending to server:', message, data);
      console.warn('Exception:', exception);
    }
  }
  return false;
}

function emit(message, err, data) {
  console.log('Emmitting', message, err, data);
  var cbs = callbacks[message];
  if (!cbs) {
    console.log('No listener registered for', message);
    return false;
  };
  for (var i = 0, len = cbs.length; i < len; i++) {
    cbs[i](err, data);
  }
  return true;
}

var server = {
  init: function () {
    setupSocket();
  },
  register: function (name) {
    sendToServer('register', {name: name});
  },
  unregister: function () {
    sendToServer('unregister');
  },
  startMoving: function () {
    sendToServer('start_moving');
  },
  left: function (down) {
    sendToServer('left_' + (down ? 'down' : 'up'));
  },
  right: function (down) {
    sendToServer('right_' + (down ? 'down' : 'up'));
  },
  on: function (message, callback) {
    if (!callbacks[message]) {
      callbacks[message] = [];
    }
    callbacks[message].push(callback);
  }
};

module.exports = server;
