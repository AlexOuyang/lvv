//
//  TransformedPrimitive.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/14/14.
//
//

#ifndef __CSE168_Rendering__TransformedPrimitive__
#define __CSE168_Rendering__TransformedPrimitive__

#include "Core.h"
#include "Primitive.h"
#include "AnimatedTransform.h"

class TransformedPrimitive : public Primitive {
public:
    
    TransformedPrimitive(const std::shared_ptr<Primitive>& primitive, const Transform& transform);
    TransformedPrimitive(const std::shared_ptr<Primitive>& primitive, const AnimatedTransform& transform);
    ~TransformedPrimitive();
    
    void                        setPrimitive(const std::shared_ptr<Primitive>& primitive);
    std::shared_ptr<Primitive>  getPrimitive() const;
    
    void        setMaterial(Material* material);
    
    void        setTransform(const Transform& transform);
    void        setTransform(const AnimatedTransform& transform);
    Transform   getTransform() const;
    
    virtual bool canIntersect() const;
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    virtual AABB getBoundingBox() const;
    
private:
    std::shared_ptr<Primitive>  _primitive;
    Material*                   _material;
    AnimatedTransform           _worldToPrimitive;
};


#endif /* defined(__CSE168_Rendering__TransformedPrimitive__) */
