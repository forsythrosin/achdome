#version 330 core

in vec2 sphericalPos;
in vec3 cartesianPos;
uniform float time;

out vec4 color;

// c0: Main logo circle
// c1: Rounded edge at thetaStart
// c2: Rounded edge at thetaEnd
// c3: Upper rounded egde at vertical shape
// c4: Lower rounded egde at vertical shape



void main() {

  float t = time * 2 - 5;

  // for increasing t:
  // 0 < t < 1: draw only vertical line from nothing
  // 1 < t < 1 + maxAngle: draw a from only vertical line
  // 1 + maxAngle < t < 2 + maxAngle: remove vertical line to form rotation
  // maxAngle + 2 < t: rotate ccw.

  
  float pi = 3.14159265359;
  
  vec3 planeNormal = normalize(vec3(0.0, 0.0, -1.0));
  vec3 yAxis = cross(planeNormal, vec3(1.0, 0.0, 0.0));
  vec3 xAxis = cross(yAxis, planeNormal);
  
  vec2 texCoord = vec2(dot(cartesianPos, xAxis),
                       dot(cartesianPos, yAxis));





  float logoRadius = 3;
  float wormWidth = logoRadius * 0.28;
  float maxAngle = 27*pi/16;

  if (t > maxAngle + 2) {
    t = 2*pi + 2*(maxAngle + 2) - t;
  }

  /*  if (t < 0) {
    t = 0;
    }*/
  


  float verticalTopRatio = max(0.0, min(t, 1));
  float verticalBottomRatio = max(0.0, min(t - maxAngle - 1, 1));
  float arcAngle = max(0.0, min(t - 1, maxAngle));  
  
  float thetaStart = mod(max(0.0, t - maxAngle - 2), pi*2);
  float theta = mod(atan(texCoord.y, texCoord.x), pi*2);
  float thetaStartDiff = mod(theta - thetaStart, pi*2);

  vec2 c0Pos = vec2(0.0, 0.0);
  vec2 c1Pos = c0Pos + logoRadius*vec2(cos(thetaStart), sin(thetaStart));
  vec2 c2Pos = c0Pos + logoRadius*vec2(cos(thetaStart + arcAngle), sin(thetaStart + arcAngle));

  vec2 c3Pos = c0Pos + logoRadius*vec2(1.0, -1.0*(1.0 - verticalTopRatio));
  vec2 c4Pos = c0Pos + logoRadius*vec2(1.0, -1.0*(1.0 - verticalBottomRatio));

  float insideVerticalShape = min(min(step(texCoord.y, c3Pos.y), 
                                      step(c4Pos.y, texCoord.y)),
                                  min(step(texCoord.x, c3Pos.x + wormWidth*0.5),
                                      step(c3Pos.x - wormWidth*0.5, texCoord.x)));
  
  float distFromC0 = distance(c0Pos, texCoord);
  float distFromC1 = distance(c1Pos, texCoord);
  float distFromC2 = distance(c2Pos, texCoord);
  float distFromC3 = distance(c3Pos, texCoord);
  float distFromC4 = distance(c4Pos, texCoord);

  float insideAngle = step(thetaStartDiff, arcAngle);


  float visibleC1 = step(1, t);
  float visibleC2 = step(1, t);
  float visibleC3 = step(t, maxAngle + 2);
  float visibleC4 = step(t, maxAngle + 2);

  float insideC1 = visibleC1 * step(distFromC1, wormWidth*0.5);
  float insideC2 = visibleC2 * step(distFromC2, wormWidth*0.5);

  float insideC3 = visibleC3 * step(distFromC3, wormWidth*0.5);
  float insideC4 = visibleC4 * step(distFromC4, wormWidth*0.5);

  float insideCircleContour = min(step(logoRadius - wormWidth/2, distFromC0),  step(distFromC0, logoRadius + wormWidth/2));
  float insideArc = min(insideAngle, insideCircleContour);

  float inside = max(max(max(max(max(insideArc, insideC1), insideC2), insideC3), insideC4), insideVerticalShape);
  
  color = vec4(1.0, 0.0, 0.4, inside);
}
