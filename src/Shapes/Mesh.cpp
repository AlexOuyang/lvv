//
//  Mesh.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#include "Mesh.h"

Mesh::Mesh()
: _verticesCount(0), _trianglesCount(0), _vertices(nullptr), _triangles(nullptr),
_alphaTexture(nullptr) {
    
}

Mesh::~Mesh() {
    if (_vertices) {
        delete[] _vertices;
    }
    if (_triangles) {
        delete[] _triangles;
    }
}

void Mesh::setVertices(int count, Vertex* vertices) {
    _verticesCount = count;
    _vertices = vertices;
}

void Mesh::setTriangles(int count, Triangle* triangles) {
    _trianglesCount = count;
    _triangles = triangles;
}

void Mesh::setAlphaTexture(Texture* texture) {
    _alphaTexture = texture;
}

int Mesh::getTrianglesCount() const {
    return _trianglesCount;
}

Triangle* Mesh::getTriangles() const {
    return _triangles;
}

AABB Mesh::getBoundingBox() const {
    AABB bound;
    
    for (int i = 0; i < _verticesCount; ++i) {
        bound = AABB::Union(bound, _vertices[i].position);
    }
    return bound;
}

bool Mesh::canIntersect() const {
    return false;
}

void Mesh::refine(std::vector<Shape*> &refined) const {
    refined.reserve(_trianglesCount);
    for (int i = 0; i < _trianglesCount; ++i) {
        refined.push_back(&_triangles[i]);
    }
}