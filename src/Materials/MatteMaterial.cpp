//
//  MatteMaterial.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "MatteMaterial.h"

MatteMaterial::MatteMaterial() : _color(1) {
    
}

MatteMaterial::MatteMaterial(const Spectrum& color) : _color(color) {

}

MatteMaterial::~MatteMaterial() {
    
}

void MatteMaterial::setColor(const vec3& color) {
    _color = color;
}

Spectrum MatteMaterial::evaluateBSDF(const vec3&, const vec3&) const {
    return _color;
}

Spectrum MatteMaterial::sampleBSDF(const vec3&, vec3*, BxDFType) const {
    return Spectrum(0);
}