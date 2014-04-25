//
//  SkyLight.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include "SkyLight.h"

SkyLight::SkyLight(const Spectrum& spectrum) : Light(), _spectrum(spectrum) {
    
}

SkyLight::~SkyLight() {
    
}

Spectrum SkyLight::le(const Ray &) const {
    return _spectrum;
}

Spectrum SkyLight::sampleL(const vec3&, float, const LightSample&, vec3*, VisibilityTester*) const {
    return Spectrum(0);
}