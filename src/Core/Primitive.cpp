//
//  Primitive.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/15/14.
//
//

#include "Primitive.h"

#include <sstream>

int Primitive::nextPrimitiveId = 1;

Primitive::Primitive() : _primitiveId(nextPrimitiveId++), _name() {
}

Primitive::~Primitive() {
    
}

bool Primitive::canIntersect() const {
    return true;
}

void Primitive::refine(std::vector<std::shared_ptr<Primitive>> &) const {
    abort();
}

void Primitive::fullyRefine(std::vector<std::shared_ptr<Primitive>>& refined) {
    std::vector<std::shared_ptr<Primitive>> todo;

    todo.push_back(this->shared_from_this());
    while (todo.size()) {
        std::shared_ptr<Primitive> current = todo.back();
        todo.pop_back();
        
        if (current->canIntersect()) {
            refined.push_back(current);
        } else {
            current->refine(todo);
        }
    }
}

AreaLight* Primitive::getAreaLight() const {
    return nullptr;
}