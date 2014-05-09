//
//  Matte.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "Matte.h"

Matte::Matte() : _color(1.f) {
    
}

Matte::Matte(const Spectrum& color) : _color(color) {

}

Matte::~Matte() {
    
}

void Matte::setColor(const vec3& color) {
    _color = Spectrum(color);
}

Spectrum Matte::evaluateBSDF(const vec3&, const vec3&,
                             const Intersection&) const {
    return _color;
}

Spectrum Matte::sampleBSDF(const vec3&, vec3* wi, const Intersection& intersection,
                           BxDFType type) const {
    if (!(type & BSDFDiffuse)) {
        return Spectrum(0.0f);
    }
    
    *wi = normalize(surfaceToWorld(cosineSampleHemisphere(), intersection));
    return _color;
}