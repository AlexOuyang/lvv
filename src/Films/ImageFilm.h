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
    
    static std::shared_ptr<ImageFilm> Load(const rapidjson::Value& value,
                                           const vec2& resolution);
    
    ImageFilm(const vec2& res);
    virtual ~ImageFilm();
    
    void setFilename(const std::string& filename);
    const std::string& getFilename() const;
    
    virtual void addSample(const CameraSample &sample, const Spectrum &L, float weight=1.0f);
    virtual void clear();
    
    void writeToFile(const std::string& filename="");
    
private:
    std::string         _filename;
    std::vector<vec3>   _buffer;
};

#endif /* defined(__CSE168_Rendering__ImageFilm__) */
