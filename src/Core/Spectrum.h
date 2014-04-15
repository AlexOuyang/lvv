//
//  Spectrum.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef __CSE168_Rendering__Spectrum__
#define __CSE168_Rendering__Spectrum__

#include "Core.h"

class Spectrum {
public:
    
    static const float Epsilon;
    
    Spectrum(float value=0.0);
    Spectrum(const vec3& color);
    ~Spectrum();
    
    void setColor(const vec3& color);
    int getIntColor() const;
    
    bool isBlack() const;
    
    Spectrum& operator+=(const Spectrum& s);
    Spectrum operator*(const Spectrum& s) const;
    Spectrum operator*(float v) const;
    
private:
    vec3    _color;
};

Spectrum operator*(float v, const Spectrum& s);

#endif /* defined(__CSE168_Rendering__Spectrum__) */
