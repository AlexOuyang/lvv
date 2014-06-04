//
//  Mesh.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#include "Mesh.h"

Mesh::Mesh()
: _verticesCount(0), _vertices(nullptr), _indicesCount(0), _indices(nullptr), _alphaTexture() {
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
    _indicesCount = count;
    _indices = indices;
}

void Mesh::setAlphaTexture(const std::shared_ptr<Texture>& texture) {
    _alphaTexture = texture;
}

void Mesh::generateTangents() {
    // Init tangents to zero vectors
    bool hasUVs = false;
    for (uint_t i = 0; i < _verticesCount; ++i) {
        _vertices[i].tangentU = vec3(0.f);
        _vertices[i].tangentV = vec3(0.f);
        hasUVs |= _vertices[i].texCoord != vec2(0.f);
    }
    
    uint_t trianglesCount = _indicesCount / 3;
    for (uint_t i = 0; i < trianglesCount; ++i) {
        Vertex
        *p0 = &_vertices[_indices[i*3+0]],
        *p1 = &_vertices[_indices[i*3+1]],
        *p2 = &_vertices[_indices[i*3+2]];
        
        vec3 tangentU, tangentV;
        
        if (!hasUVs) {
            // Generate dummy tangents based on simple axis
            vec3 normal = cross(p1->position - p0->position, p2->position - p0->position);
            
            if (abs(normal.y) > 1.0f-0.00001f) {
                tangentU = vec3(1.0f, 0.0f, 0.0f);
                tangentV = vec3(0.0f, 0.0f, 1.0f);
            } else {
                tangentU = normalize(cross(vec3(0.0f, 1.0f, 0.0f), normal));
                tangentV = cross(normal, tangentU);
            }
        } else {
            // Generate tangents based on uv coords
            vec3 q1 = p1->position - p0->position;
            vec3 q2 = p2->position - p0->position;
            float s1 = p1->texCoord.s - p0->texCoord.s;
            float t1 = p1->texCoord.t - p0->texCoord.t;
            float s2 = p2->texCoord.s - p0->texCoord.s;
            float t2 = p2->texCoord.t - p0->texCoord.t;
            
            float r = 1.0f / (s1 * t2 - s2 * t1);
            vec3 sdir((t2 * q1.x - t1 * q2.x) * r, (t2 * q1.y - t1 * q2.y) * r,
                      (t2 * q1.z - t1 * q2.z) * r);
            vec3 tdir((s1 * q2.x - s2 * q1.x) * r, (s1 * q2.y - s2 * q1.y) * r,
                      (s1 * q2.z - s2 * q1.z) * r);
            
            tangentU = normalize(sdir);
            tangentV = normalize(tdir);
        }
        
        p0->tangentU += tangentU; p0->tangentV += tangentV;
        p1->tangentU += tangentU; p1->tangentV += tangentV;
        p2->tangentU += tangentU; p2->tangentV += tangentV;
    }
    
    // Normalize tangents
    for (uint_t i = 0; i < _verticesCount; ++i) {
        _vertices[i].tangentU = normalize(_vertices[i].tangentU);
        _vertices[i].tangentV = normalize(_vertices[i].tangentV);
    }
}

int Mesh::getTrianglesCount() const {
    return _indicesCount/3;
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
    uint_t trianglesCount = _indicesCount/3;
    refined.reserve(trianglesCount);
    for (int i = 0; i < trianglesCount; ++i) {
        refined.push_back(getTriangle(i));
    }
}