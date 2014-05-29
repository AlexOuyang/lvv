//
//  Camera.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef CSE168_Rendering_Camera_h
#define CSE168_Rendering_Camera_h

#include "CameraSample.h"
#include "Ray.h"
#include "Film.h"

class Camera {
public:
    
    static std::shared_ptr<Camera> Load(const rapidjson::Value& value);
    
    Camera();
    virtual ~Camera();
    
    virtual float generateRay(const CameraSample& sample, Ray* ray) = 0;
    
    void setName(const std::string& name);
    const std::string& getName() const;
    
    void setFilm(const std::shared_ptr<Film>& film);
    std::shared_ptr<Film> getFilm() const;
    
private:
    std::string             _name;
    std::shared_ptr<Film>   _film;
};

#endif
