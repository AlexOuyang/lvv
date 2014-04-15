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
    
    bool intersect(const Ray& ray, Intersection* intersection) const;
    bool intersectP(const Ray& ray) const;
    
    std::vector<Light*> lights;
    Aggregate*          aggregate;
};

#endif /* defined(__CSE168_Rendering__Scene__) */