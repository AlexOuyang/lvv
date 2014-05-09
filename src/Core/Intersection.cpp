//
//  Intersection.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include "Intersection.h"

Intersection::Intersection() :
t(INFINITY), rayEpsilon(Core::Epsilon), point(), normal(), uv(0),
tangentU(0.f), tangentV(0.f),
material(nullptr), primitive(nullptr) {
    
}

Intersection::~Intersection() {
    
}