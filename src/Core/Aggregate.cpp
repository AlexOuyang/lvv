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

Aggregate& Aggregate::operator<<(Primitive* primitive) {
    addPrimitive(primitive);
    return *this;
}