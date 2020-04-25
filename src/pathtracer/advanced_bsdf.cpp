#include "bsdf.h"

#include <algorithm>
#include <iostream>
#include <utility>
#include "spectral_distribution.h"

using std::max;
using std::min;
using std::swap;

namespace CGL {

// Mirror BSDF //

Spectrum MirrorBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum MirrorBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO:
  // Implement MirrorBSDF
    
    BSDF::reflect(wo, wi);
    *pdf = 1.0f;
    return this->reflectance / abs_cos_theta(*wi);
}

// Microfacet BSDF //

double MicrofacetBSDF::G(const Vector3D& wo, const Vector3D& wi) {
  return 1.0 / (1.0 + Lambda(wi) + Lambda(wo));
}

double MicrofacetBSDF::D(const Vector3D& h) {
  // TODO: proj3-2, part 3
  // Compute Beckmann normal distribution function (NDF) here.
  // You will need the roughness alpha.
    
    double theta = getTheta(h);
    
    double exponent = (-1.0) * pow(tan(theta), 2.0) / pow(this->alpha, 2.0);
    double denominator = PI * pow(this->alpha, 2.0) * pow(cos(theta), 4.0);
    
    return exp(exponent) / denominator;
  //return std::pow(cos_theta(h), 100.0);;
}

double helperFres (double n, double k, double costheta) {
    
    double nk2 = (pow(n, 2.0) + pow(k, 2.0));
    double costheta2 = pow(costheta, 2.0);
    double twoncos = (2.0 * n * costheta);
    
    double RS = (nk2 - twoncos + costheta2) / (nk2 + twoncos + costheta2);
    
    double RP = ((nk2 * costheta2) - twoncos + 1.0) / ((nk2 * costheta2) + twoncos + 1.0);
    
    return (RS + RP) / 2.0;
    
}

Spectrum MicrofacetBSDF::F(const Vector3D& wi) {
  // TODO: proj3-2, part 3
  // Compute Fresnel term for reflection on dielectric-conductor interface.
  // You will need both eta and etaK, both of which are Spectrum.
    Vector3D n = this->eta;
    Vector3D k = this->k;
    double cosWI = cos_theta(wi);
    
    Spectrum nk2 = (n * n + k * k);
    double costhet = pow(cosWI, 2.0);
    Spectrum costheta2 = Spectrum(costhet, costhet, costhet);
    Spectrum twoncos = (2.0 * n * cosWI);
    
    Spectrum RS = (nk2 - twoncos + costheta2 ) / (nk2 + twoncos + costheta2);
    
    Spectrum RP = ((nk2 * costhet) - twoncos + Spectrum(1.0, 1.0, 1.0)) / ((nk2 * costhet) + twoncos + Spectrum(1.0, 1.0, 1.0));
    
    return (RS + RP) / 2.0;
    

//    return Spectrum(helperFres(n.x, k.x, cosWI), helperFres(n.y, k.y, cosWI), helperFres(n.z, k.z, cosWI));
//
//  return Spectrum();
}



Spectrum MicrofacetBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  // TODO: proj3-2, part 3
  // Implement microfacet model here.
    //my checks
    if (wo.z <= 0 || wi.z <= 0) {
        return Spectrum();
    }
    
    //old
    Vector3D h = wo + wi;
    h.normalize();
    
    Spectrum temp = F(wi) * G(wo, wi) * D(h) / (4.0 * wo.z * wi.z);

    if (temp.x <= 0 || temp.y <= 0 || temp.z <= 0) {
        return Spectrum();
    }
    
    return temp;
}

Spectrum MicrofacetBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO: proj3-2, part 3
  // *Importance* sample Beckmann normal distribution function (NDF) here.
  // Note: You should fill in the sampled direction *wi and the corresponding *pdf,
  //       and return the sampled BRDF value.
    Vector2D sample = sampler.get_sample();
    double thetat = atan(sqrt((-1.0) * pow(this->alpha, 2.0) * log(1.0 - sample.x)));
    float phi = 2.0 * PI * sample.y;
    
    
    double xval = sin(thetat) * cos(phi);
    double yval = sin(thetat) * sin(phi);
    double zval = cos(thetat); // maybe use absolute value
    Vector3D h = Vector3D(xval, yval, zval);
    //wi
    *wi = (-1.0) * wo + (2.0 * dot(wo, h) * h); //check this
    
    if (dot(h, wo) <= 0.0 || wo.z <= 0.0 || wi->z <= 0.0) {
        *pdf = 0.0001;
        return Spectrum();
    }
    
    float exponent = (-1.0) * pow(tan(getTheta(h)), 2.0) / pow(this->alpha, 2.0);
    
    float Ptheta = (2.0 * sin_theta(h)) * exp(exponent) / (pow(this->alpha, 2.0) * pow(cos_theta(h), 3.0));
    float Pphi = 1.0 / (2.0 * PI);
    float PWofH = Ptheta * Pphi / sin_theta(h);
    //pdf
    *pdf = PWofH / (4.0 * dot(*wi, h));
    
    //std::cout << PWofH << "\n";
    
  //*wi = cosineHemisphereSampler.get_sample(pdf); //placeholder
  return MicrofacetBSDF::f(wo, *wi);
}

// Refraction BSDF //

Spectrum RefractionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum RefractionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO:
  // Implement RefractionBSDF
  return Spectrum();
}

// Glass BSDF //

Spectrum GlassBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlassBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO:
  // Compute Fresnel coefficient and use it as the probability of reflection
  // - Fundamentals of Computer Graphics page 305
    if (!BSDF::refract(wo, wi, this->ior)) {
        BSDF::reflect(wo, wi);
        *pdf = 1.0f;
        return this->reflectance / abs_cos_theta(*wi);
    } else {
        float R0 = pow((1.0 - (this->ior))/(1.0 + (this->ior)), 2.0);
        float Rtheta = R0 + (1.0 - R0) * pow((1.0 - abs_cos_theta(*wi)), 5.0);
        //std::cout << Rtheta << "\n";
        if (coin_flip(Rtheta)) {
            BSDF::reflect(wo, wi);
            *pdf = Rtheta;
            return *pdf * this->reflectance / abs_cos_theta(*wi);
        } else {
            BSDF::refract(wo, wi, this->ior);
            *pdf = 1.0 - Rtheta;
            float nIOR;
            wo.z > 0? nIOR = ior : nIOR = 1.0 / ior;
            return *pdf * this->transmittance / abs_cos_theta(*wi) / pow(nIOR, 2.0);
        }
    }
    
  return Spectrum();
}



// Glowing BSDF //

double GlowingBSDF::G(const Vector3D& wo, const Vector3D& wi) {
  return 1.0 / (1.0 + Lambda(wi) + Lambda(wo));
}

double GlowingBSDF::D(const Vector3D& h) {
    double theta = getTheta(h);
    
    double exponent = (-1.0) * pow(tan(theta), 2.0) / pow(this->alpha, 2.0);
    double denominator = PI * pow(this->alpha, 2.0) * pow(cos(theta), 4.0);
    
    return exp(exponent) / denominator;
}

Spectrum GlowingBSDF::f(const Vector3D& wo, const Vector3D& wi) {
    if (wo.z <= 0 || wi.z <= 0) {
        return Spectrum();
    }
    
    Vector3D h = wo + wi;
    h.normalize();
    
    Spectrum reflected = F(wi) * G(wo, wi) * D(h) / (4.0 * wo.z * wi.z);

    if (reflected.x <= 0 || reflected.y <= 0 || reflected.z <= 0) {
        return Spectrum();
    }
    
    //using default of 5000K, will change
    SpectralDistribution blackbody = SpectralDistribution(10);
    Spectrum emitted = blackbody.toRGB();
    //std::cout << emitted.r << ", " << emitted.g << ", " << emitted.b << std::endl;
    return emitted;
}

Spectrum GlowingBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
    // *Importance* sample Beckmann normal distribution function (NDF) here.
      Vector2D sample = sampler.get_sample();
      double thetat = atan(sqrt((-1.0) * pow(this->alpha, 2.0) * log(1.0 - sample.x)));
      float phi = 2.0 * PI * sample.y;
      
      double xval = sin(thetat) * cos(phi);
      double yval = sin(thetat) * sin(phi);
      double zval = cos(thetat);
      Vector3D h = Vector3D(xval, yval, zval);
      
      *wi = (-1.0) * wo + (2.0 * dot(wo, h) * h); //check this
      
      if (dot(h, wo) <= 0.0 || wo.z <= 0.0 || wi->z <= 0.0) {
          *pdf = 0.0001;
          return Spectrum();
      }
      
      float exponent = (-1.0) * pow(tan(getTheta(h)), 2.0) / pow(this->alpha, 2.0);
      
      float Ptheta = (2.0 * sin_theta(h)) * exp(exponent) / (pow(this->alpha, 2.0) * pow(cos_theta(h), 3.0));
      float Pphi = 1.0 / (2.0 * PI);
      float PWofH = Ptheta * Pphi / sin_theta(h);
      
      *pdf = PWofH / (4.0 * dot(*wi, h));
      
      //*wi = cosineHemisphereSampler.get_sample(pdf); //placeholder
      return GlowingBSDF::f(wo, *wi);
}

//normal Fresnel, should update
Spectrum GlowingBSDF::F(const Vector3D& wi) {
    Vector3D n = this->eta;
    Vector3D k = this->k;
    double cosWI = cos_theta(wi);
    
    Spectrum nk2 = (n * n + k * k);
    double costhet = pow(cosWI, 2.0);
    Spectrum costheta2 = Spectrum(costhet, costhet, costhet);
    Spectrum twoncos = (2.0 * n * cosWI);
    
    Spectrum RS = (nk2 - twoncos + costheta2 ) / (nk2 + twoncos + costheta2);
    
    Spectrum RP = ((nk2 * costhet) - twoncos + Spectrum(1.0, 1.0, 1.0)) / ((nk2 * costhet) + twoncos + Spectrum(1.0, 1.0, 1.0));
    
    return (RS + RP) / 2.0;
}




void BSDF::reflect(const Vector3D& wo, Vector3D* wi) {
  // TODO:
  // Implement reflection of wo about normal (0,0,1) and store result in wi.
    *wi = wo * Vector3D(-1.0, -1.0, 1.0);
}

bool BSDF::refract(const Vector3D& wo, Vector3D* wi, float ior) {
  // TODO:
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When dot(wo,n) is positive, then wo corresponds to a
  // ray entering the surface through vacuum.

    float nIOR;
    
    wo.z > 0? nIOR = 1.0 / ior : nIOR = ior;
    
    float zCalc = 1 - (pow(nIOR, 2.0) * (1-pow(wo.z, 2.0)));
    
    if (zCalc < 0) {
        return false;
    }
    
    float xVal = wo.x * (-1.0) * nIOR;
    float yVal = wo.y * (-1.0) * nIOR;
    
    float pos;
    
    wo.z > 0? pos = -1.0 : pos = 1.0;
    
    float zVal = pos * sqrt(zCalc);
    
    *wi = Vector3D(xVal, yVal, zVal);
    
  return true;
}

} // namespace CGL
