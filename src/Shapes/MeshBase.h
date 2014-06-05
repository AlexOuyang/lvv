//
//  MeshBase.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/5/14.
//
//

#ifndef __CSE168_Rendering__MeshBase__
#define __CSE168_Rendering__MeshBase__

#include "Core/Core.h"
#include "Core/Shape.h"
#include "Core/Texture.h"
#include "Vertex.h"
#include "Shapes/Triangle.h"

#include <vector>

class MeshBase : public Shape {
public:
    
    friend class Triangle;    
    
    MeshBase();
    virtual ~MeshBase();
    
    void setMaterials(const std::vector<std::shared_ptr<Material>>& materials);
    
    void setVertices(int count, Vertex* vertices);
    void setIndices(int count, uint_t* indices);
    void setMaterialIndices(uint_t* indices);
    void setAlphaTexture(const std::shared_ptr<Texture>& texture);
    
    virtual void generateTangents(bool useUVs=true);
    
    int getTrianglesCount() const;
    
    const Vertex* getVertex(uint_t index) const;
    virtual AABB getBoundingBox() const;
    virtual bool canIntersect() const;
    
    static void GenerateTangents(bool useUVs,
                                 Vertex* vertices,
                                 uint_t verticesCount,
                                 uint_t* indices, uint_t indicesCount);
    
protected:
    int                                     _verticesCount;
    Vertex*                                 _vertices;
    int                                     _indicesCount;
    uint_t*                                 _indices;
    uint_t*                                 _materialIndices;
    std::vector<std::shared_ptr<Material>>  _materials;
    std::shared_ptr<Texture>                _alphaTexture;
};

#endif /* defined(__CSE168_Rendering__MeshBase__) */
