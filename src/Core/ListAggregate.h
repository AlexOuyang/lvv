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
    
    virtual void addPrimitive(const std::shared_ptr<Primitive>& primitive);
    virtual void preprocess();
    
    virtual std::shared_ptr<Primitive> findPrimitive(const std::string& name);
    virtual void removePrimitive(const std::string& name);
    virtual const std::vector<std::shared_ptr<Primitive>> getPrimitives() const;
    
    virtual AABB getBoundingBox() const;
    
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    
private:
    std::vector<std::shared_ptr<Primitive>> _primitives;
};

#endif /* defined(__CSE168_Rendering__ListAggregate__) */
