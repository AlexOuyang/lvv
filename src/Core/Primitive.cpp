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

Primitive::Primitive() : primitiveId(nextPrimitiveId++), name() {
}

Primitive::~Primitive() {
    
}

bool Primitive::canIntersect() const {
    return true;
}

void Primitive::refine(std::vector<Primitive*> &) const {
    abort();
}

void Primitive::fullyRefine(std::vector<Primitive*> &refined) {
    std::vector<Primitive*> todo;

    todo.push_back(this);
    while (todo.size()) {
        Primitive* current = todo.back();
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