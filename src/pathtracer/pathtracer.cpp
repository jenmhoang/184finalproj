#include "pathtracer.h"

#include "scene/light.h"
#include "scene/sphere.h"
#include "scene/triangle.h"


using namespace CGL::SceneObjects;

namespace CGL {

PathTracer::PathTracer() {
  gridSampler = new UniformGridSampler2D();
  hemisphereSampler = new UniformHemisphereSampler3D();

  tm_gamma = 2.2f;
  tm_level = 1.0f;
  tm_key = 0.18;
  tm_wht = 5.0f;
}

PathTracer::~PathTracer() {
  delete gridSampler;
  delete hemisphereSampler;
}

void PathTracer::autofocus(Vector2D loc) {
  Ray r = camera->generate_ray(loc.x / sampleBuffer.w, loc.y / sampleBuffer.h);
  Intersection isect;
  bvh->intersect(r, &isect);
  camera->focalDistance = isect.t;
}

void PathTracer::set_frame_size(size_t width, size_t height) {
  sampleBuffer.resize(width, height);
  sampleCountBuffer.resize(width * height);
}

void PathTracer::clear() {
  bvh = NULL;
  scene = NULL;
  camera = NULL;
  sampleBuffer.clear();
  sampleCountBuffer.clear();
  sampleBuffer.resize(0, 0);
  sampleCountBuffer.resize(0, 0);
}

void PathTracer::write_to_framebuffer(ImageBuffer &framebuffer, size_t x0,
                                      size_t y0, size_t x1, size_t y1) {
  sampleBuffer.toColor(framebuffer, x0, y0, x1, y1);
}

Spectrum
PathTracer::estimate_direct_lighting_hemisphere(const Ray &r,
                                                const Intersection &isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // For this function, sample uniformly in a hemisphere.

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // w_out points towards the source of the ray (e.g.,
  // toward the camera if this is a primary ray)
  const Vector3D &hit_p = r.o + r.d * isect.t;
  const Vector3D &w_out = w2o * (-r.d);

  // This is the same number of total samples as
  // estimate_direct_lighting_importance (outside of delta lights). We keep the
  // same number of samples for clarity of comparison.
  int num_samples = scene->lights.size() * ns_area_light;
  Spectrum L_out;

  // TODO (Part 3): Write your sampling loop here
  // TODO BEFORE YOU BEGIN
  // UPDATE `est_radiance_global_illumination` to return direct lighting instead of normal shading 
    //UniformHemisphereSampler3D sampler = UniformHemisphereSampler3D();
    
    for (int i = 0; i < num_samples; i++) {
        
        Vector3D sample = hemisphereSampler->get_sample();
        Vector3D w_in = o2w * sample; // from object to elsewhere
        Ray r = Ray(hit_p + (EPS_D * w_in), w_in);
        Intersection nIsect;
        
        if (this->bvh->intersect(r, &nIsect)) {
            Spectrum bsdf = isect.bsdf->f(w_out, sample);
            Spectrum emission = nIsect.bsdf->get_emission();
            
            //for each sample, multiply bsdf with emission/radiance with cosine
            L_out += bsdf * emission * sample.z;
        }
    }
    
    //multiply by 2 pi (uniform pdf) and then divide by num_samples
    L_out *= (2 * M_PI / num_samples);
    
    return L_out;
    
  //return Spectrum(1.0);
}

Spectrum
PathTracer::estimate_direct_lighting_importance(const Ray &r,
                                                const Intersection &isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // To implement importance sampling, sample only from lights, not uniformly in
  // a hemisphere.

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // w_out points towards the source of the ray (e.g.,
  // toward the camera if this is a primary ray)
  const Vector3D &hit_p = r.o + r.d * isect.t;
  const Vector3D &w_out = w2o * (-r.d);
  Spectrum L_out;
    
  //glow output
  //Spectrum G_out;
    
    
    for (auto light: scene->lights) {
        
        int nums = 0;
        Spectrum L_temp = Spectrum(0.0,0.0,0.0);
        
        for (int i = 0; i < ns_area_light; i ++) {
            
            nums++;
            
            Vector3D w_in;
            float lightDist;
            float pdf;
            
            Spectrum emission = light->sample_L(hit_p, &w_in, &lightDist, &pdf);
            Vector3D sample = w2o * w_in;
            Spectrum bsdf = isect.bsdf->f(w_out, sample);
            
            if (sample.z <= 0) {
                continue;
            }
            
            Ray nRay = Ray(hit_p + (EPS_D * w_in), w_in);
            nRay.max_t = lightDist - EPS_F;
            Intersection nIsect;
            
            if (!this->bvh->intersect(nRay, &nIsect)) {
                L_temp += bsdf * emission * sample.z / pdf;
                //emission = nIsect.bsdf->get_emission();
            }
            
            if (light->is_delta_light()) {
                break;
            }
        }
        
        L_out += L_temp * (1.0 / nums);
    }
    
    //importance sample glowing objects (should we just make mesh lights?)
//    for (auto obj : scene->objects) {
//        Spectrum G_temp = obj->get_bsdf()->get_emission();
//        if (G_temp.norm() > 0) {
//            G_out += G_temp;
//        }
//    }
    
    return L_out;
}


Spectrum PathTracer::zero_bounce_radiance(const Ray &r,
                                          const Intersection &isect) {
  // TODO: Part 3, Task 2
  // Returns the light that results from no bounces of light
    return isect.bsdf->get_emission();

  //return Spectrum(1.0);
}

Spectrum PathTracer::one_bounce_radiance(const Ray &r,
                                         const Intersection &isect) {
  // TODO: Part 3, Task 3
  // Returns either the direct illumination by hemisphere or importance sampling
  // depending on `direct_hemisphere_sample`
    //return estimate_direct_lighting_hemisphere(r, isect);
    return estimate_direct_lighting_importance(r, isect);
  //return Spectrum(1.0);
}

Spectrum PathTracer::at_least_one_bounce_radiance(const Ray &r,
                                                  const Intersection &isect) {
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  Vector3D hit_p = r.o + r.d * isect.t;
  Vector3D w_out = w2o * (-r.d);

  Spectrum L_out(0, 0, 0);
    if (r.depth == 0) {
        return L_out;
    }
    
    L_out = one_bounce_radiance(r, isect);

    //for next trial
    double prob = 0.6;
    Vector3D sample;
    float pdf;
    Spectrum bsdf = isect.bsdf->sample_f(w_out, &sample, &pdf);
    
    Vector3D w_in = o2w * sample;
    Ray nRay = Ray(hit_p + (EPS_D * w_in), w_in);
    Intersection nIsect;
    
    if (r.depth < 2) {
        return L_out;
    } else {
        nRay.depth = r.depth - 1;
    }
    
    if (coin_flip(prob) && this->bvh->intersect(nRay, &nIsect) && sample.z > 0) {
        L_out += at_least_one_bounce_radiance(nRay, nIsect) * bsdf * sample.z / (pdf * prob);
    }

  return L_out;
}

Spectrum PathTracer::est_radiance_global_illumination(const Ray &r) {
  Intersection isect;
  Spectrum L_out;

  // You will extend this in assignment 3-2.
  // If no intersection occurs, we simply return black.
  // This changes if you implement hemispherical lighting for extra credit.

  if (!bvh->intersect(r, &isect))
    return L_out;

  // The following line of code returns a debug color depending
  // on whether ray intersection with triangles or spheres has
  // been implemented.

  // REMOVE THIS LINE when you are ready to begin Part 3.
  //L_out = (isect.t == INF_D) ? debug_shading(r.d) : normal_shading(isect.n);

  // TODO (Part 3): Return the direct illumination.
    return zero_bounce_radiance(r, isect) + at_least_one_bounce_radiance(r, isect);
  // TODO (Part 4): Accumulate the "direct" and "indirect"
  // parts of global illumination into L_out rather than just direct

  //return L_out;
}

void PathTracer::raytrace_pixel(size_t x, size_t y) {

  // TODO (Part 1.1):
  // Make a loop that generates num_samples camera rays and traces them
  // through the scene. Return the average Spectrum.
  // You should call est_radiance_global_illumination in this function.

    
//    Spectrum result = Spectrum(0.0, 0.0, 0.0);
//
//    for (int i = 0; i < ns_aa; i++) {
//        Vector2D sample = UniformGridSampler2D().get_sample();
//        Ray r = camera->generate_ray((x + sample.x) / sampleBuffer.w, (y + sample.y) / sampleBuffer.h);
//        r.depth = max_ray_depth;
//        result += est_radiance_global_illumination(r);
//
//    }
//    result = result * (1.0 / ns_aa); // make sure to do double division
//    sampleBuffer.update_pixel(result, x, y);

    
  // TODO (Part 5):
  // Modify your implementation to include adaptive sampling.
  // Use the command line parameters "samplesPerBatch" and "maxTolerance"

    int num_samples = ns_aa;          // total samples to evaluate
    //Vector2D origin = Vector2D(x, y); // bottom left corner of the pixel
    
    Spectrum result = Spectrum(0.0, 0.0, 0.0);
    
    int n = 0;
    float s1 = 0.0;
    float s2 = 0.0;
    
    for (int i = 0; i < num_samples; i++) {
        
        Vector2D sample = UniformGridSampler2D().get_sample();
        Ray r = camera->generate_ray((x + sample.x) / sampleBuffer.w, (y + sample.y) / sampleBuffer.h);
        r.depth = max_ray_depth;
        
        Spectrum curr = est_radiance_global_illumination(r);
        result += curr;
        
        n++;
        s1 += curr.illum();
        s2 += pow(curr.illum(), 2);
        
        if (i % samplesPerBatch == 0) {
            float var = (1.0 / (n - 1)) * (s2 - (pow(s1, 2) / n));
            float I = 1.96 * sqrt(var) / sqrt(n);
            if ( I <= maxTolerance * (s1 / n)) {
                break;
            }
        }
        
    }
    
    result = result * (1.0 / n); // make sure to do double division
    sampleBuffer.update_pixel(result, x, y);
    sampleCountBuffer[x + y * sampleBuffer.w] = n;
    
//  sampleBuffer.update_pixel(Spectrum(0.2, 1.0, 0.8), x, y);

}

} // namespace CGL
