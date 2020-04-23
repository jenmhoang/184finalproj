//
//  spectral_distribution.h
//  Assignment3
//
//  Created by Ethan Buttimer on 4/22/20.
//

#ifndef spectral_distribution_h
#define spectral_distribution_h

#include "CGL.h"
#include "vector3D.h"
#include <vector>

using std::vector;

namespace CGL {
    class SpectralDistribution {
        public:
            SpectralDistribution(float temp);
        
            //may want a destructor?
            //~SpectralDistribution(){};
        
            Spectrum toRGB();
            Spectrum toXYZ();
        
        private:
            int intensity;
            float T;
            vector<float> rel_distribution;
        
            float atWavelength(float lambda);
            Spectrum rgbFromXYZ(Spectrum xyz_color);
    };
}
#endif /* spectral_distribution_h */
