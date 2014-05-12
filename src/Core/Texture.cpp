//
//  Texture.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/8/14.
//
//

#include "Texture.h"

Texture::~Texture() {
    
}

float Texture::evaluateFloat(const vec2&) const {
    return 0.f;
}

vec3 Texture::evaluateVec3(const vec2&) const {
    return vec3(0.f);
}

vec2 Texture::wrap(vec2 pos) const {
    return glm::mod(pos, vec2(1.f, 1.f));
}

UniformFloatTexture::UniformFloatTexture(float value) : Texture(), _value(value) {
    
}

UniformFloatTexture::~UniformFloatTexture() {
    
}

void UniformFloatTexture::setValue(float value) {
    _value = value;
}

float UniformFloatTexture::getValue() const {
    return _value;
}

float UniformFloatTexture::evaluateFloat(const vec2&) const {
    return _value;
}

vec3 UniformFloatTexture::evaluateVec3(const vec2&) const {
    return vec3(_value);
}

UniformVec3Texture::UniformVec3Texture(const vec3& value) : Texture(), _value(value) {
    
}

UniformVec3Texture::~UniformVec3Texture() {
    
}

void UniformVec3Texture::setValue(const vec3& value) {
    _value = value;
}

vec3 UniformVec3Texture::getValue() const {
    return _value;
}

float UniformVec3Texture::evaluateFloat(const vec2&) const {
    return glm::length(_value);
}

vec3 UniformVec3Texture::evaluateVec3(const vec2&) const {
    return _value;
}

FloatTexture::FloatTexture() : Texture(),
_data(nullptr), _width(0), _height(0), _scaleFactor(1.f) {
    
}

FloatTexture::~FloatTexture() {
    
}

void FloatTexture::setData(int width, int height, float* data) {
    _width = width;
    _height = height;
    _data = data;
}

void FloatTexture::setScaleFactor(float scale) {
    _scaleFactor = scale;
}

float FloatTexture::evaluateFloat(const vec2& p) const {
    if (!_data) {
        return 0.f;
    }
    vec2 pos = wrap(vec2(p.s, 1.f-p.t));
    return _data[(int)(pos.y*_height) * _width + (int)(pos.x*_width)] * _scaleFactor;
}

vec3 FloatTexture::evaluateVec3(const vec2& pos) const {
    return vec3(evaluateFloat(pos));
}

IntTexture::IntTexture() : Texture(),
_data(nullptr), _width(0), _height(0), _scaleFactor(1.f) {
    
}

IntTexture::~IntTexture() {
    
}

void IntTexture::setData(int width, int height, uint32_t* data) {
    _width = width;
    _height = height;
    _data = data;
}

void IntTexture::setScaleFactor(const vec3& scale) {
    _scaleFactor = scale;
}

float IntTexture::evaluateFloat(const vec2& pos) const {
    return length(evaluateVec3(pos));
}

vec3 IntTexture::evaluateVec3(const vec2& p) const {
    if (!_data) {
        return vec3(0.f);
    }
    vec2 pos = wrap(vec2(p.s, 1.f-p.t));
    uint32_t pixel = _data[(int)(pos.y*_height) * _width + (int)(pos.x*_width)];
    const uint8_t* components = (const uint8_t*)&pixel;
    vec3 value = vec3((float)components[2]/255.f,
                      (float)components[1]/255.f,
                      (float)components[0]/255.f
                      );
    return value * _scaleFactor;
}