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
#include "SceneNode.h"

class TransformedPrimitive : public Primitive, public SceneNode {
public:
    
    TransformedPrimitive(const std::shared_ptr<Primitive>& primitive, const Transform& transform);
    TransformedPrimitive(const std::shared_ptr<Primitive>& primitive, const AnimatedTransform& transform);
    ~TransformedPrimitive();
    
    void                        setPrimitive(const std::shared_ptr<Primitive>& primitive);
    std::shared_ptr<Primitive>  getPrimitive() const;
    
    void        setMaterial(Material* material);
    
    virtual bool canIntersect() const;
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    virtual AABB getBoundingBox() const;
    
private:
    std::shared_ptr<Primitive>  _primitive;
    Material*                   _material;
};


#endif /* defined(__CSE168_Rendering__TransformedPrimitive__) */
