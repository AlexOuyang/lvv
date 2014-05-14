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
    Triangle(Vertex* a, Vertex* b, Vertex* c);
    ~Triangle();
    
    void setMesh(Mesh* mesh);
    void setVertices(Vertex* v1, Vertex* v2, Vertex* v3);
    
    Vertex* getVertex(int num) const;
    
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    virtual AABB getBoundingBox() const;

private:
    Vertex* _vertices[3];
    Mesh*   _mesh;
};

#endif /* defined(__CSE168_Rendering__Triangle__) */