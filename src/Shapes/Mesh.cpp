//
//  Mesh.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#include "Mesh.h"

Mesh::Mesh()
: verticesCount(0), trianglesCount(0), vertices(nullptr), triangles(nullptr),
alphaTexture(nullptr) {
    
}

Mesh::~Mesh() {
    if (vertices) {
        delete[] vertices;
    }
    if (triangles) {
        delete[] triangles;
    }
}

AABB Mesh::getBoundingBox() const {
    AABB bound;
    
    for (int i = 0; i < verticesCount; ++i) {
        bound = AABB::Union(bound, vertices[i].position);
    }
    return bound;
}

bool Mesh::canIntersect() const {
    return false;
}

void Mesh::refine(std::vector<Shape*> &refined) const {
    refined.reserve(trianglesCount);
    for (int i = 0; i < trianglesCount; ++i) {
        refined.push_back(&triangles[i]);
    }
}