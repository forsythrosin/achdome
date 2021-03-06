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

function setupSocket(subProtocol) {
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

  function onerror(error) {
    console.warn("Websocket error", error);
  }

  function createSocket() {
    console.log('Connecting to server');
    if (typeof MozWebSocket != "undefined") {
      socket = new MozWebSocket(get_appropriate_ws_url(), subProtocol);
    } else {
      socket = new WebSocket(get_appropriate_ws_url(), subProtocol);
    }
    socket.onopen = onopen;
    socket.onclose = onclose;
    socket.onmessage = onmessage;
    socket.onerror = onerror;
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

var server;

server = {
  init: function (subProtocol) {
    subProtocol = subProtocol || "ach"; // Or whatever
    setupSocket(subProtocol);
  },
  sendToServer: function(message, data) {
    sendToServer(message, data);
  },
  on: function (message, callback) {
    var messages = message.split(" ");
    messages.forEach(function (m) {
      console.log('Register ', m);
      if (!callbacks[m]) {
        callbacks[m] = [];
      }
      callbacks[m].push(callback);
    });
    return server;
  }
};

module.exports = server;
