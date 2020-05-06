//
//  temp_map.cpp
//  pathtracer
//
//  Created by Ethan Buttimer on 5/3/20.
//

#include <stdio.h>
#include "temp_map.h"

using std::cout;
using std::endl;
using std::max;

namespace CGL {
    float ConstTempMap::temp_at(const Vector3D& pos) {
        return t;
    }

    float GradientTempMap::temp_at(const Vector3D& pos) {
        Vector3D vec = (start_pos - end_pos);
        double t = dot((pos - start_pos), vec);
        Vector3D r = start_pos + (t * vec.unit()); //corresponding point on vector
        double alpha = (start_pos - r).norm() / vec.norm(); //dist betw start and point on vec
        return alpha * end_t + (1. - alpha) * start_t;
    }

    float NoiseTempMap::temp_at(const Vector3D& pos) {
        double noise = pn.noise(pos.x * fineness, pos.y * fineness, pos.z * fineness);
        return noise * high_t + (1. - noise) * low_t;
    }

}// namespace CGL
