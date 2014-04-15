//
//  Sphere.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef __CSE168_Rendering__Sphere__
#define __CSE168_Rendering__Sphere__

#include "Core/Core.h"
#include "Core/Shape.h"

class Sphere : public Shape {
public:
    Sphere();
    virtual ~Sphere();
    
    void setCenter(const vec3& center);
    void setRadius(float radius);
    
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    virtual AABB getBoundingBox() const;
    
private:
    vec3   _center;
    float   _radius;
};

#endif /* defined(__CSE168_Rendering__Sphere__) */
