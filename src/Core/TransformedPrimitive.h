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
#include "Transform.h"

class TransformedPrimitive : public Primitive {
public:
    
    TransformedPrimitive(Primitive* primitive, const Transform& transform);
    ~TransformedPrimitive();
    
    void setPrimitive(Primitive* primitive);
    Primitive* getPrimitive() const;
    void setTransform(const Transform& transform);
    const Transform& getTransform() const;
    
    virtual bool canIntersect() const;
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    virtual AABB getBoundingBox() const;
    
private:
    Primitive*  _primitive;
    Transform   _primitiveToWorld;
    Transform   _worldToPrimitive;    
};


#endif /* defined(__CSE168_Rendering__TransformedPrimitive__) */
