//
//  Triangle.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#ifndef __CSE168_Rendering__Triangle__
#define __CSE168_Rendering__Triangle__

#include "Core/Core.h"
#include "Core/Shape.h"
#include "Vertex.h"

class Mesh;

class Triangle : public Shape {
public:
    
    Triangle();
    Triangle(uint_t a, uint_t b, uint_t c);
    ~Triangle();
    
    void setMesh(const std::shared_ptr<const Mesh>& mesh);
    void setMaterial(const std::shared_ptr<Material>& material);
    void setVertices(uint_t a, uint_t b, uint_t c);
    
    const Vertex* getVertex(int num) const;
    
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    virtual AABB getBoundingBox() const;
    
    bool hasMaterial() const;

private:
    uint_t                      _vertices[3];
    std::shared_ptr<const Mesh> _mesh;
    std::shared_ptr<Material>   _material;
};

#endif /* defined(__CSE168_Rendering__Triangle__) */