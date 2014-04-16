//
//  GeometricPrimitive.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/14/14.
//
//

#ifndef __CSE168_Rendering__GeometricPrimitive__
#define __CSE168_Rendering__GeometricPrimitive__

#include "Core.h"
#include "Primitive.h"
#include "Shape.h"
#include "Material.h"

class GeometricPrimitive : public Primitive {
public:
    
    GeometricPrimitive(Shape* shape, Material* material);
    ~GeometricPrimitive();
    
    virtual bool canIntersect() const;
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    virtual AABB getBoundingBox() const;
    
    virtual void refine(std::vector<Primitive*> &refined) const;
    
private:
    Shape*      _shape;
    Material*   _material;
};

#endif /* defined(__CSE168_Rendering__GeometricPrimitive__) */
