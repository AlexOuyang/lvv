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
    
    GeometricPrimitive(const std::shared_ptr<Shape>& shape,
                       const std::shared_ptr<Material>& material,
                       AreaLight* areaLight=nullptr);
    ~GeometricPrimitive();
    
    std::shared_ptr<Shape> getShape() const;
    void    setMaterial(const std::shared_ptr<Material>& material);
    
    virtual bool canIntersect() const;
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    virtual AABB getBoundingBox() const;
    
    virtual void refine(std::vector<std::shared_ptr<Primitive>> &refined) const;

    void setAreaLight(AreaLight* areaLight);
    virtual AreaLight* getAreaLight() const;
    
private:
    std::shared_ptr<Shape>      _shape;
    std::shared_ptr<Material>   _material;
    AreaLight*                  _areaLight;
};

#endif /* defined(__CSE168_Rendering__GeometricPrimitive__) */
