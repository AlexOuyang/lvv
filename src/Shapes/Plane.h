//
//  Plane.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef __CSE168_Rendering__Plane__
#define __CSE168_Rendering__Plane__

#include "Core/Core.h"
#include "Core/Shape.h"

class Plane : public Shape {
public:
    Plane();
    virtual ~Plane();
    
    void setOrigin(const vec3& origin);
    void setNormal(const vec3& normal);
    
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    virtual AABB getBoundingBox() const;
    
private:
    vec3   _origin;
    vec3  _normal;
    float   _d;
};

#endif /* defined(__CSE168_Rendering__Plane__) */
