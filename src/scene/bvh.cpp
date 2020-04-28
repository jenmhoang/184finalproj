#include "bvh.h"

#include "CGL/CGL.h"
#include "triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL {
namespace SceneObjects {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  primitives = std::vector<Primitive *>(_primitives);
  root = construct_bvh(primitives.begin(), primitives.end(), max_leaf_size);
}

BVHAccel::~BVHAccel() {
  if (root)
    delete root;
  primitives.clear();
}

BBox BVHAccel::get_bbox() const { return root->bb; }

void BVHAccel::draw(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->draw(c, alpha);
    }
  } else {
    draw(node->l, c, alpha);
    draw(node->r, c, alpha);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->drawOutline(c, alpha);
    }
  } else {
    drawOutline(node->l, c, alpha);
    drawOutline(node->r, c, alpha);
  }
}

BVHNode *BVHAccel::construct_bvh(std::vector<Primitive *>::iterator start,
                                 std::vector<Primitive *>::iterator end,
                                 size_t max_leaf_size) {

  // TODO (Part 2.1):
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

    //curr bounding box
    BBox bbox;
    //count of items
    int num = 0;
    //centroid to be averaged
    Vector3D cent = Vector3D();
    
    //count how many items there are
    for (auto p = start; p != end; p++) {
        num++;
        BBox bb = (*p)->get_bbox();
        bbox.expand(bb);
        cent += bb.centroid();
    }
    
    //node to be used
    BVHNode *node = new BVHNode(bbox);
    
    // if there are few enough primitives, make node
    if (num <= max_leaf_size) {
        node->start = start;
        node->end = end;
        node->l = NULL;
        node->r = NULL;
    } else {
        //set up left and right vectors
        vector<Primitive *>* left = new vector<Primitive *>();
        vector<Primitive *>* right = new vector<Primitive *>();
        //average centroids
        cent = cent / num;
        //calculate axis
        int axis;
        //calculate length of each axis
        Vector3D diff = bbox.extent;
        
        //figure out which axis is longest
        if (diff.x > diff.y && diff.x > diff.z) {
            axis = 1;
        } else if (diff.y > diff.x && diff.y > diff.z){
            axis = 2;
        } else {
            axis = 3;
        }
        
        //loop through primitives and divide based on axis
        for (auto p = start; p != end; p++) {
            if (axis == 1) {
                if ((*p)->get_bbox().centroid().x < cent.x) {
                    (*left).push_back(*p);
                } else {
                    (*right).push_back(*p);
                }
            } else if (axis == 2) {
                if ((*p)->get_bbox().centroid().y < cent.y) {
                    (*left).push_back(*p);
                } else {
                    (*right).push_back(*p);
                }
            } else {
                if ((*p)->get_bbox().centroid().z < cent.z) {
                     (*left).push_back(*p);
                } else {
                    (*right).push_back(*p);
                }
            }
        }
        node->l = construct_bvh((*left).begin(), (*left).end(), max_leaf_size);
        node->r = construct_bvh((*right).begin(), (*right).end(), max_leaf_size);
    }
    
    
    return node;
}

bool BVHAccel::has_intersection(const Ray &ray, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.

//  XXX ORIGINAL
//  for (auto p : primitives) {
//    total_isects++;
//    if (p->has_intersection(ray))
//      return true;
//  }
//  return false;
    double rmin = ray.min_t;
    double rmax = ray.max_t;
    
    if (!node->bb.intersect(ray, rmin, rmax)) {
        return false;
    }
    
    if (node->isLeaf()) {
        cout << "in the leaf intersect";
        bool hit = false;
        for (auto p = node->start; p != node->end; p++) {
            total_isects++;
            hit = (*p)->has_intersection(ray) || hit;
            return hit;
        }
    }
    
    bool hit1 = has_intersection(ray, node->l);
    bool hit2 = has_intersection(ray, node->r);
    
    return hit1 || hit2;
}

bool BVHAccel::intersect(const Ray &ray, Intersection *i, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.

    double rmin = ray.min_t;
    double rmax = ray.max_t;
    
    if (!node->bb.intersect(ray, rmin, rmax)) {
//        cout << "didn't intersect";
        return false;
    }
    
    //cout << "MADE IT";
    
    if (node->isLeaf()) {
        bool hit = false;
        for (auto p = node->start; p != node->end; p++) {
            total_isects++;
            hit = (*p)->intersect(ray, i) || hit;
        }
        return hit;
    }
    
    bool hit1 = intersect(ray, i, node->l);
    bool hit2 = intersect(ray, i, node->r);
    
    return hit1 || hit2;
//XXX ORIGINAL
//  bool hit = false;
//  for (auto p : primitives) {
//    total_isects++;
//    hit = p->intersect(ray, i) || hit;
//  }
//  return hit;
//}

} // namespace SceneObjects
} // namespace CGL
}
