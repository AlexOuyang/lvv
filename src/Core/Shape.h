//
//  Shape.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/14/14.
//
//

#ifndef __CSE168_Rendering__Shape__
#define __CSE168_Rendering__Shape__

#include <vector>

#include "Core.h"
#include "Intersection.h"
#include "AABB.h"

class Shape {
public:
    
    virtual ~Shape();
    
    virtual AABB getBoundingBox() const = 0;
    
    virtual bool canIntersect() const;
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    
    virtual void refine(std::vector<Shape*> &refined) const;
};

#endif /* defined(__CSE168_Rendering__Shape__) */
