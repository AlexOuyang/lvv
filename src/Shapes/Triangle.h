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

class Triangle : public Shape {
public:
    
    Triangle();
    Triangle(Vertex* a, Vertex* b, Vertex* c);
    ~Triangle();
    
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    virtual AABB getBoundingBox() const;
    
    Vertex* vertices[3];
};

#endif /* defined(__CSE168_Rendering__Triangle__) */