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
#include "Lights/AreaLight.h"

#include <string>
#include <vector>

class Primitive {
public:
    
    static int nextPrimitiveId;

    Primitive();
    virtual ~Primitive();

    virtual AABB getBoundingBox() const = 0;
    
    virtual bool canIntersect() const;
    virtual bool intersect(const Ray& ray, Intersection* intersection) const = 0;
    virtual bool intersectP(const Ray& ray) const = 0;
    
    virtual void refine(std::vector<Primitive*> &refined) const;
    void fullyRefine(std::vector<Primitive*> &refined);
    
    virtual AreaLight* getAreaLight() const;
    
    int getPrimitiveId() const { return _primitiveId; }
    
    void setName(const std::string& name) { _name = name; }
    const std::string& getName() const { return _name; }

private:
    int         _primitiveId;
    std::string _name;
};

#endif