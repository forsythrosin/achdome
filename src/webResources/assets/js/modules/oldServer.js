var server = {},
    socket_di, connected;

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

server.connect = function() {
  if (typeof MozWebSocket != "undefined") {
    socket_di = new MozWebSocket(get_appropriate_ws_url(), "sgct");
  } else {
    socket_di = new WebSocket(get_appropriate_ws_url(), "sgct");
  }

  socket_di.onopen = function() {
    connected = 1;
    socket_di.send("connected");
  }

  socket_di.onclose = function() {
    connected = 0;

    setTimeout(function () {
      // Connection has closed so try to reconnect every 1 second.
      server.connect();
    },  1000);
  }

  socket_di.onmessage = function got_packet(event) {
    console.log('Message received without receiver:', event);
  }

  return {
    send: function(message) {
      try {
        socket_di.send(message);
        console.log('Message sent:', message);
      } catch (e) {
        console.warn('Error while sending message:', e);
      }
    },
    onmessage: function(callback) {
      socket_di.onmessage = function(event) {
        console.log('Message received:', event);
        callback(event.data);
      };
    }
  };
};

module.exports = server;
