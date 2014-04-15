//
//  Mesh.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#include "Mesh.h"

Mesh::Mesh()
: verticesCount(0), trianglesCount(0), vertices(nullptr), triangles(nullptr) {
    
}

Mesh::~Mesh() {
    if (vertices) {
        delete[] vertices;
    }
    if (triangles) {
        delete[] triangles;
    }
}

bool Mesh::intersect(const Ray& ray, Intersection* intersection) const {
    bool intersect = false;
    
    for (int i = 0; i < trianglesCount; ++i) {
        if (triangles[i].intersect(ray, intersection)) {
            intersect = true;
        }
    }
    return intersect;
}

bool Mesh::intersectP(const Ray& ray) const {
    for (int i = 0; i < trianglesCount; ++i) {
        if (triangles[i].intersectP(ray)) {
            return true;
        }
    }
    return false;
}

AABB Mesh::getBoundingBox() const {
    AABB bound;
    
    for (int i = 0; i < verticesCount; ++i) {
        bound = AABB::Union(bound, vertices[i].position);
    }
    return bound;
}