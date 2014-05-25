//
//  ImageFilm.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/24/14.
//
//

#ifndef __CSE168_Rendering__ImageFilm__
#define __CSE168_Rendering__ImageFilm__

#include "Core/Core.h"
#include "Core/Film.h"

class ImageFilm : public Film {
public:
    
    ImageFilm(const vec2& res);
    virtual ~ImageFilm();
    
    virtual void addSample(const CameraSample &sample, const Spectrum &L, float weight=1.0f);
    
    void saveToFile(const std::string& filename);
    
private:
    std::vector<vec3>   _buffer;
};

#endif /* defined(__CSE168_Rendering__ImageFilm__) */
