//
//  Scene.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef __CSE168_Rendering__Scene__
#define __CSE168_Rendering__Scene__

#include <vector>

#include "Core/Core.h"
#include "Aggregate.h"
#include "Volume.h"
#include "Light.h"

class Scene {
public:
    
    Scene(Aggregate* aggregate);
    ~Scene();
    
    void addLight(Light* light);
    void addPrimitive(const std::shared_ptr<Primitive>& primitive);
    void setVolume(Volume* volume);
    
    Scene& operator<<(Light* light);
    Scene& operator<<(const std::shared_ptr<Primitive>& primitive);
    
    const std::vector<Light*>&      getLights() const;
    Volume*                         getVolume() const;
    
    bool intersect(const Ray& ray, Intersection* intersection) const;
    bool intersectP(const Ray& ray) const;
    
private:
    std::vector<Light*> _lights;
    Aggregate*          _aggregate;
    Volume*             _volume;
};

#endif /* defined(__CSE168_Rendering__Scene__) */