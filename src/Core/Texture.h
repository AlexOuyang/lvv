//
//  Texture.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/8/14.
//
//

#ifndef __CSE168_Rendering__Texture__
#define __CSE168_Rendering__Texture__

#include "Core.h"

class Texture {
public:
    
    virtual ~Texture();
    
    virtual float evaluateFloat(const vec2& pos) const;
    virtual vec3 evaluateVec3(const vec2& pos) const;
    virtual vec2 wrap(vec2 pos) const;
};

class UniformFloatTexture : public Texture {
public:
    
    UniformFloatTexture(float value=0.f);
    ~UniformFloatTexture();
    
    void    setValue(float value);
    float   getValue() const;
    
    virtual float evaluateFloat(const vec2& pos) const;
    virtual vec3 evaluateVec3(const vec2& pos) const;
    
private:
    float   _value;
};

class UniformVec3Texture : public Texture {
public:
    
    UniformVec3Texture(const vec3& value=vec3(0.f));
    ~UniformVec3Texture();
    
    void    setValue(const vec3& value);
    vec3    getValue() const;
    
    virtual float evaluateFloat(const vec2& pos) const;
    virtual vec3 evaluateVec3(const vec2& pos) const;
    
private:
    vec3   _value;
};

class FloatTexture : public Texture {
public:
    
    FloatTexture();
    ~FloatTexture();
    
    void setData(int width, int height, float* data);
    void setScaleFactor(float scale);
    
    virtual float evaluateFloat(const vec2& pos) const;
    virtual vec3 evaluateVec3(const vec2& pos) const;
    
private:
    float*  _data;
    int     _width;
    int     _height;
    float   _scaleFactor;
};

class IntTexture : public Texture {
public:
    
    IntTexture();
    ~IntTexture();
    
    void setData(int width, int height, uint32_t* data);
    void setScaleFactor(const vec3& scale);
    
    virtual float evaluateFloat(const vec2& pos) const;
    virtual vec3 evaluateVec3(const vec2& pos) const;
    
private:
    uint32_t*    _data;
    int         _width;
    int         _height;
    vec3        _scaleFactor;
};

#endif /* defined(__CSE168_Rendering__Texture__) */
