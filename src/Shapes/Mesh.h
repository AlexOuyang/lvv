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
#include "Core/Texture.h"
#include "Vertex.h"
#include "Triangle.h"

#include <vector>

class Mesh : public Shape {
public:

    friend class Triangle;
    
    Mesh();
    virtual ~Mesh();
    
    void setVertices(int count, Vertex* vertices);
    void setTriangles(int count, Triangle* triangles);
    void setAlphaTexture(Texture* texture);
    
    int         getTrianglesCount() const;
    Triangle*   getTriangles() const;
    
    virtual AABB getBoundingBox() const;
    
    virtual bool canIntersect() const;

    virtual void refine(std::vector<Shape*> &refined) const;

private:
    int         _verticesCount;
    int         _trianglesCount;
    Vertex*     _vertices;
    Triangle*   _triangles;
    Texture*    _alphaTexture;
};

#endif /* defined(__CSE168_Rendering__Mesh__) */
