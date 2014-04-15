//
//  ListAggregate.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/15/14.
//
//

#ifndef __CSE168_Rendering__ListAggregate__
#define __CSE168_Rendering__ListAggregate__

#include "Core.h"
#include "Aggregate.h"
#include "AABB.h"

#include <vector>

class ListAggregate : public Aggregate {
public:
    
    ListAggregate();
    ~ListAggregate();
    
    virtual void addPrimitive(Primitive* primitive);
    
    virtual AABB getBoundingBox() const;
    
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    
private:
    std::vector<Primitive*> _primitives;
};

#endif /* defined(__CSE168_Rendering__ListAggregate__) */
