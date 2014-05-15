//
//  Matte.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "Matte.h"

Matte::Matte(const vec3& color) : _color(new UniformVec3Texture(color)) {

}

Matte::Matte(Texture* color) : _color() {
    if (color) {
        _color = color;
    } else {
        _color = new UniformVec3Texture(vec3(1.f));
    }
}

Matte::~Matte() {
    
}

void Matte::setColor(const vec3& color) {
    UniformVec3Texture* texture = dynamic_cast<UniformVec3Texture*>(_color);
    if (texture) {
        texture->setValue(color);
    }
}

Texture* Matte::getColor() const {
    return _color;
}

Spectrum Matte::evaluateBSDF(const vec3&, const vec3&,
                             const Intersection& intersection) const {
    return Spectrum(_color->evaluateVec3(intersection.uv) / (float)M_PI);
}

Spectrum Matte::sampleBSDF(const vec3&, vec3* wi, const Intersection& intersection,
                           BxDFType type) const {
    if (!(type & BSDFDiffuse)) {
        return Spectrum(0.0f);
    }
    
    *wi = normalize(surfaceToWorld(cosineSampleHemisphere(), intersection));
    return _color->evaluateVec3(intersection.uv);
}