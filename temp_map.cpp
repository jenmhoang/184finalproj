//
//  temp_map.cpp
//  pathtracer
//
//  Created by Ethan Buttimer on 5/3/20.
//

#include <stdio.h>
#include "temp_map.h"

namespace CGL {
    float ConstTempMap::temp_at(Vector3D& pos) {
        return t;
    }

    float GradientTempMap::temp_at(Vector3D& pos) {
        return start_t;
    }

    float NoiseTempMap::temp_at(Vector3D &pos) {
        return low_t;
    }

}// namespace CGL
