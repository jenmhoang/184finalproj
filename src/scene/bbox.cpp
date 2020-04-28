#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CGL {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {

  // TODO (Part 2.2):
  // Implement ray - bounding box intersection test
  // If the ray intersected the bouding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.
    Vector3D bmin = this->min;
    Vector3D bmax = this->max;
    Vector3D o = r.o;
    Vector3D d = r.d;

    double low;
    double high;
    
    if (t0 > t1) {
        low = t1;
        high = t0;
    } else {
        low = t0;
        high = t1;
    }
    
    double tx1 = (bmin.x - o.x) / d.x;
    double tx2 = (bmax.x - o.x) / d.x;
    double ty1 = (bmin.y - o.y) / d.y;
    double ty2 = (bmax.y - o.y) / d.y;
    double tz1 = (bmin.z - o.z) / d.z;
    double tz2 = (bmax.z - o.z) / d.z;
    
    double resLow = std::max({std::min(tx1, tx2), std::min(ty1, ty2), std::min(tz1, tz2)});
    double resHigh = std::min({std::max(tx1, tx2), std::max(ty1, ty2), std::max(tz1, tz2)});

    //if ((low <= resLow && resLow <= high) || (low <= resHigh && resHigh <= high)) {
        if (resLow <= resHigh) {
            if (resHigh < low || resLow > high) {
                return false;
            } else {
//                std::cout << bmin << std::endl;
                //std::cout << "low: " << resLow << "high: " << resHigh << "\n";
                return true;
            }
        } else {
            return false;
        }
    //}
    
  return false;
}

void BBox::draw(Color c, float alpha) const {

  glColor4f(c.r, c.g, c.b, alpha);

  // top
  glBegin(GL_LINE_STRIP);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
  glEnd();

  // bottom
  glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glEnd();

  // side
  glBegin(GL_LINES);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
  glEnd();

}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

} // namespace CGL
