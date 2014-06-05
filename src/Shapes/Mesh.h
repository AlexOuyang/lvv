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
#include "Shapes/MeshBase.h"
#include "Shapes/Triangle.h"

class Mesh : public MeshBase, public std::enable_shared_from_this<Mesh> {
public:
    
    Mesh();
    virtual ~Mesh();
    
    std::shared_ptr<Triangle> getTriangle(int index) const;
    
    virtual void refine(std::vector<std::shared_ptr<Shape>> &refined) const;
};

#endif /* defined(__CSE168_Rendering__Mesh__) */
