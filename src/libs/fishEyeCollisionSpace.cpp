#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <fisheyeCollisionSpace.h>
#include <wormCollision.h>
#include <wormArc.h>
#include <map>
#include <iostream>

const float FisheyeCollisionSpace::HALF_PIXEL = 0.5;

FisheyeCollisionSpace::FisheyeCollisionSpace(int radius) {
  this->size = radius*2;
  this->bitmap = new Bitmap(this->size, this->size, PixelValue::createOutsideBounds());
  clear();
}

FisheyeCollisionSpace::~FisheyeCollisionSpace() {
  delete bitmap;
}

std::vector<WormCollision> FisheyeCollisionSpace::addArcs(std::vector<WormArc> arcs) {
  // Only create at max one worm collision per worm id.
  std::map<int, WormCollision> collisions;
  
  for (WormArc arc : arcs) {
    std::vector<glm::dquat> points = getArcPoints(arc);

    for (glm::dquat point : points) {
      glm::dvec2 transformed = transform(point);
      int i = floor(transformed.x);
      int j = floor(transformed.y);

      // draw four pixels instead of one to guarantee 4-connectivity.
      for (int x = i; x <= i + 1; x++) for (int y = j; y <= j + 1; y++) {

          int wormId = arc.getWormId();
          int time = arc.getTime();

          PixelValue pv = bitmap->getPixel(x, y);
          
          if (pv.isOutsideBounds()) {
            // hit boundary.
            if (collisions.find(wormId) == collisions.end()) {
              // no collision registered for this worm yet.
              collisions.insert({wormId, WormCollision(wormId, pv.wormId, point, time)});
              std::cout << wormId << " is outside bounds at " << x << ", " << y << std::endl;
            } // else, do nothing if collision is already registered.
          } else if (pv.wormId == wormId) {
            // hit self.
            if (pv.time < time - 1) {
              // Old pixel is older than previous frame
              if (collisions.find(wormId) == collisions.end()) {
                // no collision registered for this worm yet
                collisions.insert({wormId, WormCollision(wormId, pv.wormId, point, time)});
                std::cout << wormId << " collided with itself at " << x << ", " << y << ". t1 = " << time << ", t0 = "<< pv.time << std::endl;
              } // else, do nothing if collision is already registered.
            } else {
            }// else, do nothing: (resistant to pixels drawn on current and previous frame)
          } else if (!pv.isEmpty()) {
            // hit other worm.
            if (collisions.find(wormId) == collisions.end()) {
              // no collision registered for this worm yet.
              collisions.insert({wormId, WormCollision(wormId, pv.wormId, point, time)});
              std::cout << wormId << " collided with " << pv.wormId << " at " << x << ", " << y << std::endl;
            }

            if (pv.time == time) {
              // other worm currently being drawn here too,
              // and should also get a collision, if not yet inserted.
              if (collisions.find(pv.wormId) == collisions.end()) {
                collisions.insert({wormId, WormCollision(pv.wormId, wormId, point, time)});
              }
            }
          }
          bitmap->setPixel(x, y, PixelValue(arc.getWormId(), arc.getTime()));
        }
    }
  }

  std::vector<WormCollision> v;
  for (auto it : collisions) {
    v.push_back(it.second);
  }

  //  bitmap->saveToPPM("debug.ppm");
  return v;
}

void FisheyeCollisionSpace::clear() {
  float radius = float(size)/2.0;
  float radiusSquared = radius*radius;

  for (int y = 0; y < this->size; y++) {
    for (int x = 0; x < this->size; x++) {
      float dx = (x - radius);
      float dy = (y - radius);
      float r = dx*dx + dy*dy;
      if (r < radiusSquared) {
        this->bitmap->setPixel(x, y, PixelValue::createEmpty());
      }
    }
  }
}

glm::dvec2 FisheyeCollisionSpace::transform(glm::dquat in) {
  glm::dvec3 pos = glm::mat3_cast(in) * glm::dvec3(1.0, 0.0, 0.0);

  double phi = atan2((double)pos.y, (double)pos.x);
  double theta = acos((double)pos.z);
  double fov = 2.0 * glm::pi<float>();

  float x = size * (0.5 + theta/fov*2.0 * cos(phi));
  float y = size * (0.5 + theta / fov*2.0 * sin(phi));

  return glm::dvec2(x, y);
}

std::vector<glm::dquat> FisheyeCollisionSpace::getArcPoints(WormArc arc, double start, double end) {
  glm::dquat startQuat = arc.getLerp(start);
  glm::dquat endQuat = arc.getLerp(end);

  glm::dvec2 transformedStart = transform(startQuat);
  glm::dvec2 transformedEnd = transform(endQuat);

  /*  std::cout << "transformedStart" << transformedStart.x << ", " 
             << transformedStart.y << " - "
            << transformedEnd.x << ", "
            << transformedEnd.y << ". t0=" << start << " t1=" << end << std::endl; */

  std::vector<glm::dquat> points;
  if (glm::distance(transformedStart, transformedEnd) > HALF_PIXEL && end - start > 0.001) {
    double middle = start + (end - start)/2.0;
    std::vector<glm::dquat> head = getArcPoints(arc, start, middle);
    std::vector<glm::dquat> tail = getArcPoints(arc, middle, end);

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


/**
 * Save bitmap to file
 */
void FisheyeCollisionSpace::saveBitmapToFile(std::string filename) {
  bitmap->saveToPPM(filename);
}
