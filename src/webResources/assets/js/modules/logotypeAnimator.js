var arcAngle = 27 * Math.PI / 16;
var arcGap = 2 * Math.PI - arcAngle;
var lineLength = 1;
var pathLength = arcAngle + lineLength;
var lineLimit = lineLength / pathLength;
var arcLimit = (lineLength + Math.PI) / pathLength;

var finishLoop, calcParamsFun;

var easing = {
  linear: function (t) { return t },
  easeInQuad: function (t) { return t*t },
  easeOutQuad: function (t) { return t*(2-t) },
  easeInOutQuad: function (t) { return t<.5 ? 2*t*t : -1+(4-2*t)*t },
  easeInCubic: function (t) { return t*t*t },
  easeOutCubic: function (t) { return (--t)*t*t+1 },
  easeInOutCubic: function (t) { return t<.5 ? 4*t*t*t : (t-1)*(2*t-2)*(2*t-2)+1 },
  easeInQuart: function (t) { return t*t*t*t },
  easeOutQuart: function (t) { return 1-(--t)*t*t*t },
  easeInOutQuart: function (t) { return t<.5 ? 8*t*t*t*t : 1-8*(--t)*t*t*t },
  easeInQuint: function (t) { return t*t*t*t*t },
  easeOutQuint: function (t) { return 1+(--t)*t*t*t*t },
  easeInOutQuint: function (t) { return t<.5 ? 16*t*t*t*t*t : 1+16*(--t)*t*t*t*t }
};

var set = function (path, values) {
  var str = '';
  for (var i = 0, len = values.length; i < len; i++) {
    str += ' ' + values[i];
  }
  path.setAttribute('d', str);
}

var i = 0;

var finish = function(path, length, canvas, radius, arcAngle, easeFun, callback) {
  var timer = length;
  var d = new Date();
  var center = canvas.center;
  var padded = canvas.padded;
  var prevTime = d.getTime();
  var x0 = center.x + radius * Math.cos(arcAngle);
  var y0 = center.y - radius * Math.sin(arcAngle);
  var iv = setInterval(function() {
    var newD = new Date();
    var newTime = newD.getTime();
    var timeDiff = newTime - prevTime;
    if (timeDiff > 0) {
      prevTime = newTime;
      timer -= timeDiff;
      if (timer > 0) {
        var s = easeFun((length-timer)/length);
        x1 = padded.x;
        y1 = center.y + s * radius;
        set(path, [
          'M', x0, y0,
          'A', radius, radius, 0, 1, 1, padded.x, center.y,
          'L', x1, y1
        ]);
      } else {
        x1 = padded.x;
        y1 = padded.y;
        set(path, [
          'M', x0, y0,
          'A', radius, radius, 0, 1, 1, padded.x, center.y,
          'L', x1, y1
        ]);
        if (callback) callback();
        calcParamsFun = null;
        clearInterval(iv);
      }
    }
  },0);
}

function animateLogotype(length, delay, ease, loader, callback) {
  length = length === 0 ? 0 : (length || 750);
  delay = delay || 0;
  var easeFun = (!loader && ease !== undefined && easing[ease] !== undefined) ? easing[ease] : easing['linear'];

  var radius, center, padded;

  function calcParams() {
    var svg = document.getElementById('logotype');
    if (!svg) return;
    var path = document.getElementById('logotypePath');
    var width = svg.offsetWidth;
    var height = svg.offsetHeight;
    var size = Math.min(width, height);
    var lineRatio = 0.14;
    var lineWidth = size*lineRatio;
    radius = size*(1 - lineRatio - 0.01)/2;

    center = {
      x: width/2,
      y: height/2
    }
    padded = {
      x: center.x + radius,
      y: center.y + radius
    }
    path.setAttribute('stroke-width', lineWidth);
  }

  calcParamsFun = calcParams;
  calcParams();

  setTimeout(function() {
    var firstLoop = true;
    finishLoop = false;
    var timer = length;
    var d = new Date();
    var prevTime = d.getTime();

    var iv = setInterval(function() {
      calcParams();
      path = document.getElementById('logotypePath');
      if (!path) {
        clearInterval(iv);
        return;
      }
      var newD = new Date();
      var newTime = newD.getTime();
      var timeDiff = newTime - prevTime;
      if (timeDiff > 0) {
        prevTime = newTime;
        timer -= timeDiff;
        if (loader) {
          // Loader
          if (timer < 0) {
            if (finishLoop) {
              finish(
                path,
                length / (2 * Math.PI),
                {center: center, padded: padded},
                radius, arcAngle, easeFun,
                callback
              );
              clearInterval(iv);
            }
            firstLoop = false;
            timer = length;
          } else {
            var s = easeFun((length-timer)/length);
            var angle = Math.PI * 2 * (1-s);
            var big, x0, y0;
            var x1 = center.x + radius*Math.cos(angle);
            var y1 = center.y - radius*Math.sin(angle);
            if (firstLoop) {
              if (angle > arcGap) {
                x0 = padded.x;
                y0 = center.y;
              } else {
                x0 = center.x + radius*Math.cos(angle - arcGap);
                y0 = center.y - radius*Math.sin(angle - arcGap);
              }
              big = (angle < Math.PI) ? 1 : 0;
            } else {
              big = 1;
              x0 = center.x + radius*Math.cos(angle - arcGap);
              y0 = center.y - radius*Math.sin(angle - arcGap);
            }
            set(path, ['M', x0, y0, 'A', radius, radius, 0, big, 1, x1, y1]);
          }
        } else {
          // Logotype animation
          if (timer < 0) {
            set(path, [
              'M', padded.x, padded.y,
              'L', padded.x, padded.y-radius,
              'A', radius, radius, 0, 1, 0, center.x + radius*Math.cos(arcAngle), center.y - radius*Math.sin(arcAngle)
            ]);
            if (callback) callback();
            calcParamsFun = null;
            clearInterval(iv);
          } else {
            var s = easeFun((length-timer)/length);
            if (s < lineLimit) {
              // draw line
              var part = s/lineLimit;
              set(path, [
                'M', padded.x, padded.y,
                'L', padded.x, padded.y-part*radius
              ]);
            } else {
              var angle = (s - lineLimit) / (1 - lineLimit) * arcAngle;
              var big = angle > Math.PI ? 1 : 0;
              var x = center.x + radius*Math.cos(angle);
              var y = center.y - radius*Math.sin(angle);
              set(path, [
                'M', padded.x, padded.y,
                'L', padded.x, padded.y-radius,
                'A', radius, radius, 0, big, 0, x, y
              ]);
            }
          }
        }
      }
    },0);
  }, delay);
};

module.exports = {
  animate: function (length, delay, ease, callback) {
    animateLogotype(length, delay, ease, false, callback);
  },
  animateLoader: function (length, delay, callback) {
    animateLogotype(length, delay, null, true, callback);
  },
  finishLoader: function() {
    finishLoop = true;
  },
  resize: function(callback) {
    if (calcParamsFun) {
      calcParamsFun();
    } else {
      var getType = {};
      if (callback && getType.toString.call(callback) !== '[object Function]') {
        callback = null;
      }
      animateLogotype(0, 0, null, false, callback);
    }
  }
};
