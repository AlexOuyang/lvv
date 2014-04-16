//
//  Shape.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/15/14.
//
//

#include "Shape.h"

Shape::~Shape() {
    
}

bool Shape::canIntersect() const {
    return true;
}

bool Shape::intersect(const Ray&, Intersection*) const {
    abort();
}

bool Shape::intersectP(const Ray&) const {
    abort();
}

void Shape::refine(std::vector<Shape*> &) const {
    abort();
}