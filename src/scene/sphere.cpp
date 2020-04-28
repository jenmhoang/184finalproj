#include "sphere.h"

#include <cmath>

#include "pathtracer/bsdf.h"
#include "util/sphere_drawing.h"

namespace CGL {
namespace SceneObjects {

bool Sphere::test(const Ray &r, double &t1, double &t2) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.
    
    //sphere origin and radius squared
    Vector3D so = this->o;
    double sr2 = this->r2;
    
    //ray origin and direction
    Vector3D o = r.o;
    Vector3D d = r.d;
    
    //origins of ray - sphere
    Vector3D oc = o - so;
    
    double a = dot(d, d);
    double b = dot(2.0 * oc, d);
    double c = dot(oc, oc) - sr2;
    
    double disc = sqrt((b * b) - (4.0 * a * c));
    
    double temp1 = (-b - disc) / (2.0*a);
    double temp2 = (-b + disc) / (2.0*a);
    
    if (0 <= temp1 && r.min_t <= temp1 && temp1 <= r.max_t) {
        if (0 <= temp2 && r.min_t <= temp2 && temp2 <= r.max_t) {
            if (temp1 < temp2) {
                t1 = temp1;
                t2 = temp2;
            } else {
                t1 = temp2;
                t2 = temp1;
            }
        } else {
            t1 = temp1;
        }
        r.max_t = t1;
        return true;
    } else if (0 <= temp2 && r.min_t <= temp2 && temp2 <= r.max_t) {
        t1 = temp2;
        r.max_t = t1;
        return true;
    } else {
        return false;
    }
}

bool Sphere::has_intersection(const Ray &r) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.
    double t1 = -1;
    double t2 = -1;
    return test(r, t1, t2);
}

bool Sphere::intersect(const Ray &r, Intersection *i) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.
    double time1 = -1;
    double time2 = -1;
    bool ret = test(r, time1, time2);
    if (ret && time1 >= 0) {
        i->t = time1;
        i->n = (r.at_time(time1) - (this->o)).unit();
        i->primitive = this;
        i->bsdf = get_bsdf();
    }
  return ret;
}

void Sphere::draw(const Color &c, float alpha) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color &c, float alpha) const {
  // Misc::draw_sphere_opengl(o, r, c);
}

} // namespace SceneObjects
} // namespace CGL
