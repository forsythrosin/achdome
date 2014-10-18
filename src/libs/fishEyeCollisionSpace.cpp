#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <fisheyeCollisionSpace.h>
#include <wormCollision.h>
#include <wormArc.h>
#include <cmath>
#include <map>

const float FisheyeCollisionSpace::HALF_PIXEL = 0.5;

FisheyeCollisionSpace::FisheyeCollisionSpace(int radius) {
  this->size = radius*2;
  this->bitmap = new Bitmap(this->size, this->size, PixelValue::createOutsideBounds());
}

FisheyeCollisionSpace::~FisheyeCollisionSpace() {

}

std::vector<WormCollision> FisheyeCollisionSpace::addArcs(std::vector<WormArc> arcs) {

  // Only create at max one worm collision per worm id.
  std::map<int, WormCollision> collisions;

  for (WormArc arc : arcs) {
    std::vector<glm::quat> points = getArcPoints(arc);
    for (glm::quat point : points) {

      glm::vec2 transformed = transform(point);
      int x = round(transformed.x);
      int y = round(transformed.y);

      int wormId = arc.getWormId();
      int time = arc.getTime();
      
      // todo? read from more than one pixel if worm is wider?
      // todo! worms can cross each other if they are +-45 deg and perpendicular
      PixelValue pv = bitmap->getPixel(x, y);
      
      if (pv.isOutsideBounds()) {
        // hit boundary.
        if (collisions.find(wormId) == collisions.end()) {
          // no collision registered for this worm yet.
          collisions.insert({wormId, WormCollision(wormId, pv.wormId, point)});
        } // else, do nothing if collision is already registered.
      } else if (pv.wormId == wormId) {
        // hit self.
        if (pv.time < time - 1) {
          // Old pixel is older than previous frame
          if (collisions.find(wormId) == collisions.end()) {
            // no collision registered for this worm yet.
            collisions.insert({wormId, WormCollision(wormId, pv.wormId, point)});
          } // else, do nothing if collision is already registered.
        } // else, do nothing: (resistant to pixels drawn on current and previous frame)
      } else if (!pv.isEmpty()) {
        // hit other worm.
        if (collisions.find(wormId) == collisions.end()) {
          // no collision registered for this worm yet.
          collisions.insert({wormId, WormCollision(wormId, pv.wormId, point)});
        }
          
        if (pv.time == time) {
          // other worm currently being drawn here too,
          // and should also get a collision, if not yet inserted.
          if (collisions.find(pv.wormId) == collisions.end()) {
            collisions.insert({wormId, WormCollision(pv.wormId, wormId, point)});
          }
        }
      }
      bitmap->setPixel(x, y, PixelValue(arc.getWormId(), arc.getTime()));
    }
  }

  std::vector<WormCollision> v;
  return v;
}

void FisheyeCollisionSpace::clear() {

}

glm::vec2 FisheyeCollisionSpace::transform(glm::quat in) {
  glm::vec3 pos = glm::mat3_cast(in) * glm::vec3(1.0, 0.0, 0.0);

  float phi = atan(pos.y/pos.x);
  float theta = acos(pos.z);

  float x = size/2 + theta * cos(phi);
  float y = size/2 + theta * sin(phi);

  return glm::vec2(x, y);
}



std::vector<glm::quat> FisheyeCollisionSpace::getArcPoints(WormArc arc, float start, float end) {
  glm::quat startQuat = arc.getLerp(start);
  glm::quat endQuat = arc.getLerp(end);

  glm::vec2 transformedStart = transform(startQuat);
  glm::vec2 transformedEnd = transform(endQuat);

  std::vector<glm::quat> points;
  if (glm::distance(transformedStart, transformedEnd) > HALF_PIXEL) {
    float middle = start + (end - start)/2.0;
    std::vector<glm::quat> head = getArcPoints(arc, start, middle);
    std::vector<glm::quat> tail = getArcPoints(arc, middle, end);

    for (int i = 0; i < head.size() - 1; i++) {
      points.push_back(head[i]);
    }
    for (int i = 0; i < tail.size(); i++) {
      points.push_back(tail[i]);
    }
  } else {
    points.push_back(startQuat);
    points.push_back(endQuat);
  }
  return points;
}
