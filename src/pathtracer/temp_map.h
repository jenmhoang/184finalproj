//
//  temp_map.h
//  pathtracer
//
//  Created by Ethan Buttimer on 5/3/20.
//

#ifndef temp_map_h
#define temp_map_h

#include "CGL.h"
#include "vector3D.h"
#include <vector>

using std::vector;
using std::cout;
using std::endl;

namespace CGL {
    class TempMap {
        public:
        virtual float temp_at(const Vector3D& pos) {return 3000;}
    };

    class ConstTempMap : public TempMap {
        public:
            ConstTempMap(float t) : t(t) { }
            float temp_at(const Vector3D& pos);
        
        private:
            float t;
    };

    class GradientTempMap : public TempMap {
        public:
            GradientTempMap(float start_t, float end_t, Vector3D& start_pos, Vector3D& end_pos) : start_t(start_t), end_t(end_t), start_pos(start_pos), end_pos(end_pos) { }
            float temp_at(const Vector3D& pos);
        
        private:
            float start_t;
            float end_t;
            Vector3D start_pos;
            Vector3D end_pos;
    };

    class NoiseTempMap : public TempMap {
        public:
            NoiseTempMap(float low_t, float high_t, float fineness) : low_t(low_t), high_t(high_t), fineness(fineness) { }
            float temp_at(const Vector3D& pos);
        
        private:
            float low_t;
            float high_t;
            float fineness;
    };
}
#endif /* temp_map_h */
