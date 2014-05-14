//
//  ListAggregate.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/15/14.
//
//

#include "ListAggregate.h"

#include <QDebug>

ListAggregate::ListAggregate() : _primitives() {
    
}

ListAggregate::~ListAggregate() {
    
}

void ListAggregate::addPrimitive(Primitive* primitive) {
    _primitives.push_back(primitive);
}

void ListAggregate::preprocess() {
    // Refine the primitives
    std::vector<Primitive*> refined;
    for (Primitive* p : _primitives) {
        p->fullyRefine(refined);
    }
    _primitives.swap(refined);
}

Primitive* ListAggregate::findPrimitive(const std::string& name) {
    for (Primitive* p : _primitives) {
        if (p->getName() == name) {
            return p;
        }
    }
    return nullptr;
}

void ListAggregate::removePrimitive(const std::string& name) {
    std::remove_if(_primitives.begin(), _primitives.end(), [name] (Primitive* p) {
        return p->getName() == name;
    });
}

const std::vector<Primitive*> ListAggregate::getPrimitives() const {
    return _primitives;
}

AABB ListAggregate::getBoundingBox() const {
    AABB bound;
    
    for (Primitive* p : _primitives) {
        bound = AABB::Union(bound, p->getBoundingBox());
    }
    return bound;
}

bool ListAggregate::intersect(const Ray& ray, Intersection* intersection) const {
    bool intersect = false;
    
    for (Primitive* p : _primitives) {
        intersect |= p->intersect(ray, intersection);
    }
    return intersect;
}

bool ListAggregate::intersectP(const Ray& ray) const {
    for (Primitive* p : _primitives) {
        if (p->intersectP(ray)) {
            return true;
        }
    }
    return false;
}