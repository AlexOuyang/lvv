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

#include <vector>

class Mesh : public Shape {
public:
    
    Mesh();
    virtual ~Mesh();
    
    virtual AABB getBoundingBox() const;
    
    virtual bool canIntersect() const;

    virtual void refine(std::vector<Shape*> &refined) const;
    
    int         verticesCount;
    int         trianglesCount;
    Vertex*     vertices;
    Triangle*   triangles;
};

#endif /* defined(__CSE168_Rendering__Mesh__) */
