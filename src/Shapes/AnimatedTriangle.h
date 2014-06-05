//
//  AnimatedTriangle.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/5/14.
//
//

#ifndef __CSE168_Rendering__AnimatedTriangle__
#define __CSE168_Rendering__AnimatedTriangle__

#include "Core/Core.h"
#include "Core/Shape.h"
#include "Vertex.h"

class AnimatedMesh;

class AnimatedTriangle : public Shape {
public:
    
    AnimatedTriangle();
    AnimatedTriangle(uint_t a, uint_t b, uint_t c);
    ~AnimatedTriangle();
    
    void setMesh(const std::shared_ptr<const AnimatedMesh>& mesh);
    void setMaterial(const std::shared_ptr<Material>& material);
    
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    virtual AABB getBoundingBox() const;
    
    bool hasMaterial() const;
    
private:
    uint_t                              _vertices[3];
    std::shared_ptr<const AnimatedMesh> _mesh;
    std::shared_ptr<Material>           _material;
};


#endif /* defined(__CSE168_Rendering__AnimatedTriangle__) */
