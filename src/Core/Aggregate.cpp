//
//  Aggregate.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/15/14.
//
//

#include "Aggregate.h"

Aggregate::~Aggregate() {
    
}

void Aggregate::preprocess() {
}

void Aggregate::rebuild() {
}

Aggregate& Aggregate::operator<<(const std::shared_ptr<Primitive>& primitive) {
    addPrimitive(primitive);
    return *this;
}

std::shared_ptr<Primitive> Aggregate::findPrimitive(const std::string&) {
    return std::shared_ptr<Primitive>(nullptr);
}

void Aggregate::removePrimitive(const std::string&) {
}

const std::vector<std::shared_ptr<Primitive>> Aggregate::getPrimitives() const {
    return std::vector<std::shared_ptr<Primitive>>();
}