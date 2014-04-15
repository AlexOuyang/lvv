//
//  Spectrum.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include "Spectrum.h"

const float Spectrum::Epsilon = 1.0 / (256*2);

Spectrum::Spectrum(float value) : _color(value, value, value) {
}

Spectrum::Spectrum(const vec3& color) : _color(color) {
    
}

Spectrum::~Spectrum() {
    
}

void Spectrum::setColor(const vec3& color) {
    _color = color;
}

int Spectrum::getIntColor() const {
    int value = 0;
    char* components = (char*)&value;
    
    components[2] = (char)clamp((int)(_color.r * 256), 0, 255);
    components[1] = (char)clamp((int)(_color.g * 256), 0, 255);
    components[0] = (char)clamp((int)(_color.b * 256), 0, 255);
    
    return value;
}

bool Spectrum::isBlack() const {
    return (_color.r < Epsilon) && (_color.g < Epsilon) && (_color.b < Epsilon);
}

Spectrum& Spectrum::operator+=(const Spectrum& s) {
    _color += s._color;
    return *this;
}

Spectrum Spectrum::operator*(const Spectrum& s) const {
    return Spectrum(_color * s._color);
}

Spectrum Spectrum::operator*(float v) const {
    return Spectrum(_color * v);
}

Spectrum operator*(float v, const Spectrum& s) {
    return s * v;
}