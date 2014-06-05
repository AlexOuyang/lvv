//
//  Mesh.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#include "Mesh.h"

Mesh::Mesh() {
}

Mesh::~Mesh() {
}

std::shared_ptr<Triangle> Mesh::getTriangle(int index) const {
    std::shared_ptr<Triangle> t = std::make_shared<Triangle>(_indices[index*3+0],
                                                             _indices[index*3+1],
                                                             _indices[index*3+2]);
    t->setMesh(this->shared_from_this());
    if (_materialIndices) {
        t->setMaterial(_materials[_materialIndices[index] % _materials.size()]);
    }
    return t;
}

void Mesh::refine(std::vector<std::shared_ptr<Shape>> &refined) const {
    uint_t trianglesCount = _indicesCount/3;
    refined.reserve(trianglesCount);
    for (int i = 0; i < trianglesCount; ++i) {
        refined.push_back(getTriangle(i));
    }
}