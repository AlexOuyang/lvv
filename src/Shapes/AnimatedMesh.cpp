//
//  AnimatedMesh.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/4/14.
//
//

#include "AnimatedMesh.h"

#include "Shapes/Mesh.h"

AnimatedMesh::AnimatedMesh() : _endVertices(nullptr) {
    
}

AnimatedMesh::~AnimatedMesh() {
    if (_endVertices) {
        delete[] _endVertices;
    }
}

void AnimatedMesh::setEndVertices(Vertex* vertices) {
    if (_endVertices) {
        delete[] _endVertices;
    }
    _endVertices = vertices;
}

const Vertex* AnimatedMesh::getEndVertex(uint_t index) const {
    return &_endVertices[index];
}

Vertex AnimatedMesh::getInterpolatedVertex(uint_t index, float time) const {
    Vertex vertex;
    
    const Vertex* v0 = &_vertices[index];
    const Vertex* v1 = &_endVertices[index];
    
    vertex.position = glm::mix(v0->position, v1->position, time);
    vertex.normal = normalize(glm::mix(v0->normal, v1->normal, time));
    vertex.tangentU = normalize(glm::mix(v0->tangentU, v1->tangentU, time));
    vertex.tangentV = normalize(glm::mix(v0->tangentV, v1->tangentV, time));
    vertex.texCoord = v0->texCoord;
    
    return vertex;
}

void AnimatedMesh::generateTangents(bool useUVs) {
    Mesh::GenerateTangents(useUVs, _vertices, _verticesCount, _indices, _indicesCount);
    Mesh::GenerateTangents(useUVs, _endVertices, _verticesCount, _indices, _indicesCount);
}

std::shared_ptr<AnimatedTriangle> AnimatedMesh::getAnimatedTriangle(int index) const {
    std::shared_ptr<AnimatedTriangle> t = std::make_shared<AnimatedTriangle>(_indices[index*3+0],
                                                                             _indices[index*3+1],
                                                                             _indices[index*3+2]);
    t->setMesh(this->shared_from_this());
    if (_materialIndices) {
        t->setMaterial(_materials[_materialIndices[index] % _materials.size()]);
    }
    return t;
}

AABB AnimatedMesh::getBoundingBox() const {
    AABB bound;
    
    for (int i = 0; i < _verticesCount; ++i) {
        bound = AABB::Union(bound, _vertices[i].position);
        bound = AABB::Union(bound, _endVertices[i].position);
    }
    return bound;
}

void AnimatedMesh::refine(std::vector<std::shared_ptr<Shape>> &refined) const {
    uint_t trianglesCount = _indicesCount/3;
    refined.reserve(trianglesCount);
    for (int i = 0; i < trianglesCount; ++i) {
        refined.push_back(getAnimatedTriangle(i));
    }
}