//
//  Primitive.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/14/14.
//
//

#ifndef CSE168_Rendering_Primitive_h
#define CSE168_Rendering_Primitive_h

#include "Core.h"

#include <vector>

class Primitive {
public:

    virtual ~Primitive();

    virtual AABB getBoundingBox() const = 0;
    
    virtual bool canIntersect() const;
    virtual bool intersect(const Ray& ray, Intersection* intersection) const = 0;
    virtual bool intersectP(const Ray& ray) const = 0;
    
    virtual void refine(std::vector<Primitive*> &refined) const;
    void fullyRefine(std::vector<Primitive*> &refined);
};

#endif
