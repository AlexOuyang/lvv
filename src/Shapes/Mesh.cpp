//
//  Mesh.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#include "Mesh.h"

Mesh::Mesh()
: _verticesCount(0), _trianglesCount(0), _vertices(nullptr), _indices(nullptr), _alphaTexture() {
}

Mesh::~Mesh() {
    if (_vertices) {
        delete[] _vertices;
    }
    if (_indices) {
        delete[] _indices;
    }
}

void Mesh::setVertices(int count, Vertex* vertices) {
    _verticesCount = count;
    _vertices = vertices;
}

void Mesh::setIndices(int count, uint_t* indices) {
    _trianglesCount = count;
    _indices = indices;
}

void Mesh::setAlphaTexture(const std::shared_ptr<Texture>& texture) {
    _alphaTexture = texture;
}

int Mesh::getTrianglesCount() const {
    return _trianglesCount;
}

std::shared_ptr<Triangle> Mesh::getTriangle(int index) const {
    std::shared_ptr<Triangle> t = std::make_shared<Triangle>(&_vertices[_indices[index*3+0]],
                                                             &_vertices[_indices[index*3+1]],
                                                             &_vertices[_indices[index*3+2]]);
    t->setMesh(this->shared_from_this());
    return t;
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

void Mesh::refine(std::vector<std::shared_ptr<Shape>> &refined) const {
    refined.reserve(_trianglesCount);
    for (int i = 0; i < _trianglesCount; ++i) {
        refined.push_back(getTriangle(i));
    }
}