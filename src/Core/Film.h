//
//  Film.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef __CSE168_Rendering__Film__
#define __CSE168_Rendering__Film__

#include "Core/Core.h"
#include "Core/CameraSample.h"
#include "Core/Spectrum.h"

class Film {
public:

    Film(const vec2& res);
    virtual ~Film();
    
    virtual void addSample(const CameraSample &sample, const Spectrum &L) = 0;
    
    const vec2 resolution;
};

#endif /* defined(__CSE168_Rendering__Film__) */
