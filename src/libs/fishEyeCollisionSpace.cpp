#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <fisheyeCollisionSpace.h>
#include <wormCollision.h>
#include <wormArc.h>
#include <cmath>
#include <map>
#include <iostream>

const float FisheyeCollisionSpace::HALF_PIXEL = 0.5;

FisheyeCollisionSpace::FisheyeCollisionSpace(int radius) {
  this->size = radius*2;
  this->bitmap = new Bitmap(this->size, this->size, PixelValue::createOutsideBounds());
  
  for (int y = 0; y < this->size; y++) {
    for (int x = 0; x < this->size; x++) {
      float dx = (x - radius);
      float dy = (y - radius);
      float r = sqrt(dx*dx + dy*dy);
      if (r < radius) {
        this->bitmap->setPixel(x, y, PixelValue::createEmpty());
      }
    }
  }
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
            // no collision registered for this worm yet
            collisions.insert({wormId, WormCollision(wormId, pv.wormId, point)});
          } // else, do nothing if collision is already registered.
        } else {
        }// else, do nothing: (resistant to pixels drawn on current and previous frame)
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
  for (auto it : collisions) {
    v.push_back(it.second);
  }

  return v;
}

void FisheyeCollisionSpace::clear() {

}

glm::vec2 FisheyeCollisionSpace::transform(glm::quat in) {
  glm::vec3 pos = glm::mat3_cast(in) * glm::vec3(1.0, 0.0, 0.0);

  double phi = atan2(pos.y, pos.x);
  double theta = acos(pos.z);
  double fov = 2.0 * glm::pi<float>();

  float x = size * (0.5 + theta/fov*2.0 * cos(phi));
  float y = size * (0.5 + theta/fov*2.0 * sin(phi));

  return glm::vec2(x, y);
}



std::vector<glm::quat> FisheyeCollisionSpace::getArcPoints(WormArc arc, double start, double end) {
  glm::quat startQuat = arc.getLerp(start);
  glm::quat endQuat = arc.getLerp(end);

  glm::vec2 transformedStart = transform(startQuat);
  glm::vec2 transformedEnd = transform(endQuat);

  /*  std::cout << "transformedStart" << transformedStart.x << ", " 
             << transformedStart.y << " - "
            << transformedEnd.x << ", "
            << transformedEnd.y << ". t0=" << start << " t1=" << end << std::endl; */

  std::vector<glm::quat> points;
  if (glm::distance(transformedStart, transformedEnd) > HALF_PIXEL && end - start > 0.001) {
    double middle = start + (end - start)/2.0;
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
