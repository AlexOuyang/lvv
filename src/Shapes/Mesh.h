//
//  Mesh.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#ifndef __CSE168_Rendering__Mesh__
#define __CSE168_Rendering__Mesh__

#include "Core/Core.h"
#include "Core/Shape.h"
#include "Vertex.h"
#include "Triangle.h"

class Mesh : public Shape {
public:
    
    Mesh();
    virtual ~Mesh();
    
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    virtual AABB getBoundingBox() const;
    
    int         verticesCount;
    int         trianglesCount;
    Vertex*     vertices;
    Triangle*   triangles;
};

#endif /* defined(__CSE168_Rendering__Mesh__) */
