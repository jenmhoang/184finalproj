#include "triangle.h"

#include "CGL/CGL.h"
#include "GL/glew.h"

namespace CGL {
namespace SceneObjects {

Triangle::Triangle(const Mesh *mesh, size_t v1, size_t v2, size_t v3) {
  p1 = mesh->positions[v1];
  p2 = mesh->positions[v2];
  p3 = mesh->positions[v3];
  n1 = mesh->normals[v1];
  n2 = mesh->normals[v2];
  n3 = mesh->normals[v3];
  bbox = BBox(p1);
  bbox.expand(p2);
  bbox.expand(p3);

  bsdf = mesh->get_bsdf();
}

BBox Triangle::get_bbox() const { return bbox; }

bool Triangle::has_intersection(const Ray &r) const {
  // Part 1, Task 3: implement ray-triangle intersection
  // The difference between this function and the next function is that the next
  // function records the "intersection" while this function only tests whether
  // there is a intersection.
    Vector3D a = this->p1;
    Vector3D b = this->p2;
    Vector3D c = this->p3;
    
    Vector3D o = r.o;
    Vector3D d = r.d;
    
    Vector3D E1 = b - a;
    Vector3D E2 = c - a;
    Vector3D S = o - a;
    Vector3D S1 = cross(d, E2);
    Vector3D S2 = cross(S, E1);
    
    Vector3D result = (1.0 / (dot(S1, E1))) * (Vector3D(dot(S2, E2), dot(S1, S), dot(S2, d)));
    
    double rem = 1.0 - result.y - result.z;
    
    return((0.0 <= result.x && r.min_t <= result.x && result.x <= r.max_t)
                    && (0.0 <= result.y && result.y <= 1.0)
                    && (0.0 <= result.z && result.z <= 1.0)
                    && (0.0 <= rem && rem <= 1.0));
}

bool Triangle::intersect(const Ray &r, Intersection *isect) const {
  // Part 1, Task 3:
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly
    Vector3D a = this->p1;
    Vector3D b = this->p2;
    Vector3D c = this->p3;
    
    Vector3D o = r.o;
    Vector3D d = r.d;
    
    Vector3D E1 = b - a;
    Vector3D E2 = c - a;
    Vector3D S = o - a;
    Vector3D S1 = cross(d, E2);
    Vector3D S2 = cross(S, E1);
    
    Vector3D result = (1.0 / (dot(S1, E1))) * (Vector3D(dot(S2, E2), dot(S1, S), dot(S2, d)));
    
    double rem = 1.0 - result.y - result.z;
    
    bool hasInt =  ((0.0 <= result.x && r.min_t <= result.x && result.x <= r.max_t)
                    && (0.0 <= result.y && result.y <= 1.0)
                    && (0.0 <= result.z && result.z <= 1.0)
                    && (0.0 <= rem && rem <= 1.0));
    
    if (hasInt) {
        isect->t = result.x;
        r.max_t = result.x;
        isect->n = ((rem * this->n1) + (result.y * this->n2) + (result.z * this->n3)).unit();
        isect->primitive = this;
        isect->bsdf = get_bsdf();
    }
    
    return hasInt;
}

void Triangle::draw(const Color &c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_TRIANGLES);
  glVertex3d(p1.x, p1.y, p1.z);
  glVertex3d(p2.x, p2.y, p2.z);
  glVertex3d(p3.x, p3.y, p3.z);
  glEnd();
}

void Triangle::drawOutline(const Color &c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_LINE_LOOP);
  glVertex3d(p1.x, p1.y, p1.z);
  glVertex3d(p2.x, p2.y, p2.z);
  glVertex3d(p3.x, p3.y, p3.z);
  glEnd();
}

} // namespace SceneObjects
} // namespace CGL
