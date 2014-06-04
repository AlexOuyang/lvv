//
//  Intersection.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include "Intersection.h"

#include "Core/Material.h"

Intersection::Intersection() :
t(INFINITY), rayEpsilon(Core::Epsilon), point(), normal(), uv(0),
tangentU(0.f), tangentV(0.f),
material(nullptr), primitive(nullptr) {
    
}

Intersection::~Intersection() {
    
}

void Intersection::applyNormalMapping() {
    if (!material) {
        return;
    }
    
    const Texture* normalMap = material->getNormalMap();
    
    if (!normalMap) {
        return;
    }
    
    vec3 newNormal = normalMap->evaluateVec3(uv);
    newNormal = normalize(newNormal - vec3(0.5f));
    newNormal = normalize(tangentU*newNormal.x + tangentV*newNormal.y
                          + normal*newNormal.z);
    normal = newNormal;
}