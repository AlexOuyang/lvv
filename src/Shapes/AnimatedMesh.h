//
//  AnimatedMesh.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/4/14.
//
//

#ifndef __CSE168_Rendering__AnimatedMesh__
#define __CSE168_Rendering__AnimatedMesh__

#include "Core/Core.h"
#include "Shapes/MeshBase.h"
#include "Shapes/AnimatedTriangle.h"

class AnimatedMesh : public MeshBase, public std::enable_shared_from_this<AnimatedMesh> {
public:
    
    friend class AnimatedTriangle;
    
    AnimatedMesh();
    virtual ~AnimatedMesh();
    
    void setEndVertices(Vertex* vertices);
    const Vertex* getEndVertex(uint_t index) const;
    
    Vertex getInterpolatedVertex(uint_t index, float time) const;
    
    virtual void generateTangents(bool useUVs=true);
    
    std::shared_ptr<AnimatedTriangle> getAnimatedTriangle(int index) const;
    
    virtual AABB getBoundingBox() const;
    virtual void refine(std::vector<std::shared_ptr<Shape>> &refined) const;
    
private:
    Vertex* _endVertices;
};

#endif /* defined(__CSE168_Rendering__AnimatedMesh__) */
