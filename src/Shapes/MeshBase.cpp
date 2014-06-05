//
//  MeshBase.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/5/14.
//
//

#include "MeshBase.h"

MeshBase::MeshBase() :
_verticesCount(0), _vertices(nullptr),
_indicesCount(0), _indices(nullptr),
_materialIndices(nullptr), _materials(),
_alphaTexture() {
    
}

MeshBase::~MeshBase() {
    if (_vertices) {
        delete[] _vertices;
    }
    if (_indices) {
        delete[] _indices;
    }
    if (_materialIndices) {
        delete[] _materialIndices;
    }
}

void MeshBase::setMaterials(const std::vector<std::shared_ptr<Material>>& materials) {
    _materials = materials;
}

void MeshBase::setVertices(int count, Vertex* vertices) {
    if (_vertices) {
        delete[] _vertices;
    }
    _verticesCount = count;
    _vertices = vertices;
}

void MeshBase::setIndices(int count, uint_t* indices) {
    if (_indices) {
        delete [] _indices;
    }
    _indicesCount = count;
    _indices = indices;
}

void MeshBase::setMaterialIndices(uint_t* indices) {
    if (_materialIndices) {
        delete[] _materialIndices;
    }
    _materialIndices = indices;
}

void MeshBase::setAlphaTexture(const std::shared_ptr<Texture>& texture) {
    _alphaTexture = texture;
}

void MeshBase::generateTangents(bool useUVs) {
    GenerateTangents(useUVs, _vertices, _verticesCount, _indices, _indicesCount);
}

int MeshBase::getTrianglesCount() const {
    return _indicesCount/3;
}

const Vertex* MeshBase::getVertex(uint_t index) const {
    return &_vertices[index];
}

AABB MeshBase::getBoundingBox() const {
    AABB bound;
    
    for (int i = 0; i < _verticesCount; ++i) {
        bound = AABB::Union(bound, _vertices[i].position);
    }
    return bound;
}

bool MeshBase::canIntersect() const {
    return false;
}

void MeshBase::GenerateTangents(bool useUVs, Vertex* vertices,
                            uint_t verticesCount, uint_t* indices, uint_t indicesCount) {
    // Init tangents to zero vectors
    for (uint_t i = 0; i < verticesCount; ++i) {
        vertices[i].tangentU = vec3(0.f);
        vertices[i].tangentV = vec3(0.f);
    }
    
    uint_t trianglesCount = indicesCount / 3;
    for (uint_t i = 0; i < trianglesCount; ++i) {
        Vertex
        *p0 = &vertices[indices[i*3+0]],
        *p1 = &vertices[indices[i*3+1]],
        *p2 = &vertices[indices[i*3+2]];
        
        vec3 tangentU, tangentV;
        
        bool invalidUVs = false;
        if (useUVs) {
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
        
        if (glm::isnan(tangentU.x) || glm::isnan(tangentU.y) || glm::isnan(tangentU.z)
            || glm::isnan(tangentV.x) || glm::isnan(tangentV.y) || glm::isnan(tangentV.z)) {
            invalidUVs = true;
        }
        
        if (!useUVs || invalidUVs) {
            // Generate dummy tangents based on simple axis
            vec3 normal = normalize(cross(p1->position - p0->position, p2->position - p0->position));
            
            if (abs(normal.y) > 1.0f-0.00001f) {
                tangentU = vec3(1.0f, 0.0f, 0.0f);
                tangentV = vec3(0.0f, 0.0f, 1.0f);
            } else {
                tangentU = normalize(cross(vec3(0.0f, 1.0f, 0.0f), normal));
                tangentV = cross(normal, tangentU);
            }
        }
        
        p0->tangentU += tangentU; p0->tangentV += tangentV;
        p1->tangentU += tangentU; p1->tangentV += tangentV;
        p2->tangentU += tangentU; p2->tangentV += tangentV;
    }
    
    // Normalize tangents
    for (uint_t i = 0; i < verticesCount; ++i) {
        vertices[i].tangentU = normalize(vertices[i].tangentU);
        vertices[i].tangentV = normalize(vertices[i].tangentV);
    }
}