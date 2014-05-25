//
//  Spectrum.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include "Spectrum.h"

const float Spectrum::Epsilon = 1.0f / (256.f*2.f);

Spectrum::Spectrum(float value) : _color(value, value, value) {
}

Spectrum::Spectrum(const vec3& color) : _color(color) {
    
}

Spectrum::Spectrum(int intColor) : _color(0.f, 0.f, 0.f) {
    unsigned char* components = (unsigned char*)&intColor;
    
    _color.r = ((float)components[2]) / 255.f;
    _color.g = ((float)components[1]) / 255.f;
    _color.b = ((float)components[0]) / 255.f;
}

Spectrum::~Spectrum() {
    
}

void Spectrum::setColor(const vec3& color) {
    _color = color;
}

vec3 Spectrum::getColor() const {
    return _color;
}

int Spectrum::getIntColor() const {
    int value = 0;
    unsigned char* components = (unsigned char*)&value;
    
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

Spectrum& Spectrum::operator*=(const Spectrum& s) {
    _color *= s._color;
    return *this;
}

Spectrum Spectrum::operator-() const {
    return -_color;
}

Spectrum Spectrum::operator+(const Spectrum& s) const {
    return Spectrum(_color + s._color);
}

Spectrum Spectrum::operator-(const Spectrum& s) const {
    return Spectrum(_color - s._color);
}

Spectrum Spectrum::operator*(const Spectrum& s) const {
    return Spectrum(_color * s._color);
}

Spectrum Spectrum::operator*(float v) const {
    return Spectrum(_color * v);
}

Spectrum Spectrum::exp(const Spectrum &s) {
    return glm::exp(s._color);
}

Spectrum operator*(float v, const Spectrum& s) {
    return s * v;
}