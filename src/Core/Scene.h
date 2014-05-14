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
#include "Light.h"

class Scene {
public:
    
    Scene(Aggregate* aggregate);
    ~Scene();
    
    void addLight(Light* light);
    void addPrimitive(Primitive* primitive);
    
    Scene& operator<<(Light* light);
    Scene& operator<<(Primitive* primitive);
    
    const std::vector<Light*>   getLights() const;
    
    bool intersect(const Ray& ray, Intersection* intersection) const;
    bool intersectP(const Ray& ray) const;
    
private:
    std::vector<Light*> _lights;
    Aggregate*          _aggregate;
};

#endif /* defined(__CSE168_Rendering__Scene__) */