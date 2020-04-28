#include "bsdf.h"

#include <algorithm>
#include <iostream>
#include <utility>
#include <cmath>

using std::max;
using std::min;
using std::swap;

namespace CGL {

/**
 * This function creates a object space (basis vectors) from the normal vector
 */
void make_coord_space(Matrix3x3 &o2w, const Vector3D &n) {

  Vector3D z = Vector3D(n.x, n.y, n.z);
  Vector3D h = z;
  if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z))
    h.x = 1.0;
  else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z))
    h.y = 1.0;
  else
    h.z = 1.0;

  z.normalize();
  Vector3D y = cross(h, z);
  y.normalize();
  Vector3D x = cross(z, y);
  x.normalize();

  o2w[0] = x;
  o2w[1] = y;
  o2w[2] = z;
}

/**
 * Evaluate diffuse lambertian BSDF.
 * Given incident light direction wi and outgoing light direction wo. Note
 * that both wi and wo are defined in the local coordinate system at the
 * point of intersection.
 * \param wo outgoing light direction in local space of point of intersection
 * \param wi incident light direction in local space of point of intersection
 * \return reflectance in the given incident/outgoing directions
 */
Spectrum DiffuseBSDF::f(const Vector3D &wo, const Vector3D &wi) {
  // TODO (Part 3.1):
  // This function takes in both wo and wi and returns the evaluation of
  // the BSDF for those two directions.
    return this->reflectance / M_PI;
    
  //return Spectrum(1.0);
}

/**
 * Evalutate diffuse lambertian BSDF.
 */
Spectrum DiffuseBSDF::sample_f(const Vector3D &wo, Vector3D *wi, float *pdf) {
  // TODO (Part 3.1):
  // This function takes in only wo and provides pointers for wi and pdf,
  // which should be assigned by this function.
  // After sampling a value for wi, it returns the evaluation of the BSDF
  // at (wo, *wi).
  // You can use the `f` function. The reference solution only takes two lines.
    *wi = this->sampler.get_sample(pdf);
    return DiffuseBSDF::f(wo, *wi);
  //return Spectrum(1.0);
}

//===============================================================
// Project 3-2 Code. Don't worry about these for project 3-1
//===============================================================

/**
 * Evalutate Emission BSDF (Light Source)
 */
Spectrum EmissionBSDF::f(const Vector3D &wo, const Vector3D &wi) {
  return Spectrum();
}

/**
 * Evalutate Emission BSDF (Light Source)
 */
Spectrum EmissionBSDF::sample_f(const Vector3D &wo, Vector3D *wi, float *pdf) {
  *pdf = 1.0 / PI;
  *wi = sampler.get_sample(pdf);
  return Spectrum();
}

} // namespace CGL