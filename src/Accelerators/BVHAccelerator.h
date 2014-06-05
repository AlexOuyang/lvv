//
//  BVHAccelerator.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/15/14.
//
//

#ifndef __CSE168_Rendering__BVHAccelerator__
#define __CSE168_Rendering__BVHAccelerator__

#include "Core/Core.h"
#include "Core/Ray.h"
#include "Core/Aggregate.h"
#include "Core/AABB.h"

#include <vector>

struct CompareToBucket;

class BVHAccelerator : public Aggregate {
public:
    
    friend CompareToBucket;
    
    enum SplitMethod {
        SplitMiddle,
        SplitEqualCounts,
        SplitSAH
    };
    
    BVHAccelerator(SplitMethod splitMethod=SplitSAH);
    ~BVHAccelerator();
    
    virtual void preprocess();
    virtual void rebuild();
    
    virtual void addPrimitive(const std::shared_ptr<Primitive>& primitive);

    virtual std::shared_ptr<Primitive> findPrimitive(const std::string& name);
    virtual void removePrimitive(const std::string& name);
    virtual const std::vector<std::shared_ptr<Primitive>> getPrimitives() const;
    
    virtual AABB getBoundingBox() const;
    
    virtual bool intersect(const Ray& ray, Intersection* intersection) const;
    virtual bool intersectP(const Ray& ray) const;
    
private:
    
    // Private data structures
    
    struct Node {
        Node();
        ~Node();
        
        AABB        boundingBox;
        Node*       children[2];
        uint32_t    splitDimension;
        uint32_t    primitivesOffset;
        uint32_t    primitivesCount;
    };
    
    struct BuildPrimitiveInfo {
        BuildPrimitiveInfo();
        BuildPrimitiveInfo(int primitiveIndex, const AABB& bb);
        ~BuildPrimitiveInfo();
        
        int     primitiveIndex;
        vec3    centroid;
        AABB    boundingBox;
    };
    
    struct CompareToPoint {
        CompareToPoint(uint32_t dimension, float point);
        
        bool operator()(const BuildPrimitiveInfo& p);
        
        uint32_t    dimension;
        float       point;
    };
    
    struct ComparePoints {
        ComparePoints(uint32_t dimension);
        
        bool operator()(const BuildPrimitiveInfo& a, const BuildPrimitiveInfo& b);
        
        uint32_t    dimension;
    };
    
    // Private methods
    
    Node* recursiveBuild(std::vector<BuildPrimitiveInfo>& buildData,
                         uint32_t start, uint32_t end,
                         std::vector<std::shared_ptr<Primitive>>& orderedPrimitives);
    
    SplitMethod                             _splitMethod;
    std::vector<std::shared_ptr<Primitive>> _primitives;
    Node*                                   _root;
};

#endif /* defined(__CSE168_Rendering__BVHAccelerator__) */
