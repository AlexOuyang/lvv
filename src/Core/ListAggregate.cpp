//
//  ListAggregate.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/15/14.
//
//

#include "ListAggregate.h"

ListAggregate::ListAggregate() : _primitives() {
    
}

ListAggregate::~ListAggregate() {
    
}

void ListAggregate::addPrimitive(Primitive* primitive) {
    _primitives.push_back(primitive);
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