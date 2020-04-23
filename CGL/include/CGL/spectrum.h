#ifndef CGL_SPECTRUM_H
#define CGL_SPECTRUM_H

#include "CGL.h"
#include "vector3D.h"

namespace CGL {

// Commutable scalar multiplication
inline Spectrum operator*(float s, const Spectrum &c) { return c * s; }

// Prints components
std::ostream &operator<<(std::ostream &os, const Spectrum &c);

}  // namespace CGL

#endif  // CGL_SPECTRUM_H

