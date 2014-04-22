//
//  BVHAccelerator.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/15/14.
//
//

#include <QDebug>

#include "BVHAccelerator.h"

BVHAccelerator::Node::Node()
: boundingBox(), children(), splitDimension(0), primitivesOffset(0), primitivesCount(0) {
    children[0] = children[1] = nullptr;
}

BVHAccelerator::BuildPrimitiveInfo::BuildPrimitiveInfo()
: primitiveIndex(0), centroid(), boundingBox() {
    
}

BVHAccelerator::BuildPrimitiveInfo::BuildPrimitiveInfo(int p, const AABB& bb)
: primitiveIndex(p), centroid(), boundingBox(bb) {
    centroid = 0.5f * bb.min + 0.5f * bb.max;
}

BVHAccelerator::BuildPrimitiveInfo::~BuildPrimitiveInfo() {
    
}

BVHAccelerator::CompareToPoint::CompareToPoint(uint32_t dimension, float point)
: dimension(dimension), point(point) {
    
}

bool BVHAccelerator::CompareToPoint::operator()(const BuildPrimitiveInfo& p) {
    return p.centroid[dimension] < point;
}

BVHAccelerator::ComparePoints::ComparePoints(uint32_t dimension) : dimension(dimension) {
    
}

bool BVHAccelerator::ComparePoints::operator()(const BuildPrimitiveInfo& a,
                                               const BuildPrimitiveInfo& b) {
    return a.centroid[dimension] < b.centroid[dimension];
}

BVHAccelerator::BVHAccelerator(SplitMethod splitMethod)
: _splitMethod(splitMethod), _primitives() {
    
}

BVHAccelerator::~BVHAccelerator() {
    
}

void BVHAccelerator::addPrimitive(Primitive* primitive) {
    _primitives.push_back(primitive);
}

void BVHAccelerator::preprocess() {
    // First refine the primitives
    std::vector<Primitive*> refined;
    for (Primitive* p : _primitives) {
        p->fullyRefine(refined);
    }
    _primitives.swap(refined);
    
    if (!_primitives.size()) {
        _root = nullptr;
        return;
    }
    
    // Initialize data used for building bvh
    std::vector<BuildPrimitiveInfo> buildData;
    buildData.reserve(_primitives.size());
    for (uint32_t i = 0; i < _primitives.size(); ++i) {
        buildData.push_back(BuildPrimitiveInfo(i, _primitives[i]->getBoundingBox()));
    }
    
    // Recursively build BVH tree for primitives
    std::vector<Primitive*> orderedPrimitives;
    orderedPrimitives.reserve(_primitives.size());
    _root = recursiveBuild(buildData, 0, _primitives.size(), orderedPrimitives);
    _primitives.swap(orderedPrimitives);
}

AABB BVHAccelerator::getBoundingBox() const {
    return _root ? _root->boundingBox : AABB();
}

bool BVHAccelerator::intersect(const Ray& ray, Intersection* intersection) const {
    if (!_root) {
        return false;
    }
    
    bool hit = false;
    
    // Follow ray through BVH nodes to find primitives intersections
    uint32_t todoOffset = 0;
    Node* todo[64];
    Node* currentNode = _root;
    
    float t0, t1;
    
    while (true) {
        // Check ray against current node
        if (currentNode->boundingBox.intersectP(ray, &t0, &t1)) {
            if (currentNode->primitivesCount > 0) {
                // Leaf node, check ray against primitives
                for (uint32_t i = 0; i < currentNode->primitivesCount; ++i) {
                    if (_primitives[currentNode->primitivesOffset+i]->intersect(ray, intersection)) {
                        hit = true;
                    }
                }
                if (todoOffset == 0) {
                    // Stack is empty, no more node to check
                    break;
                }
                currentNode = todo[--todoOffset];
            } else {
                // Put far BVH node onto stack, advance to near node
                if (ray.direction[currentNode->splitDimension] < 0) {
                    todo[todoOffset++] = currentNode->children[0];
                    currentNode = currentNode->children[1];
                } else {
                    todo[todoOffset++] = currentNode->children[1];
                    currentNode = currentNode->children[0];
                }
            }
        } else {
            if (todoOffset == 0) {
                // Stack is empty, no more node to check
                break;
            }
            currentNode = todo[--todoOffset];
        }
    }
    
    return hit;
}

bool BVHAccelerator::intersectP(const Ray& ray) const {
    if (!_root) {
        return false;
    }
    
    bool hit = false;
    
    // Follow ray through BVH nodes to find primitives intersections
    uint32_t todoOffset = 0;
    Node* todo[64];
    Node* currentNode = _root;
    
    float t0, t1;
    
    while (true) {
        // Check ray against current node
        if (currentNode->boundingBox.intersectP(ray, &t0, &t1)) {
            if (currentNode->primitivesCount > 0) {
                // Leaf node, check ray against primitives
                for (uint32_t i = 0; i < currentNode->primitivesCount; ++i) {
                    if (_primitives[currentNode->primitivesOffset+i]->intersectP(ray)) {
                        return true;
                    }
                }
                if (todoOffset == 0) {
                    // Stack is empty, no more node to check
                    break;
                }
                currentNode = todo[--todoOffset];
            } else {
                // Put far BVH node onto stack, advance to near node
                if (ray.direction[currentNode->splitDimension] < 0) {
                    todo[todoOffset++] = currentNode->children[0];
                    currentNode = currentNode->children[1];
                } else {
                    todo[todoOffset++] = currentNode->children[1];
                    currentNode = currentNode->children[0];
                }
            }
        } else {
            if (todoOffset == 0) {
                // Stack is empty, no more node to check
                break;
            }
            currentNode = todo[--todoOffset];
        }
    }
    
    return hit;
}

BVHAccelerator::Node* BVHAccelerator::recursiveBuild(std::vector<BuildPrimitiveInfo>& buildData,
                                                     uint32_t start, uint32_t end,
                                                     std::vector<Primitive*>& orderedPrimitives) {
    Node* node = new Node();
    
    // Compute bounding box of all primitives in current node
    for (uint32_t i = start; i < end; ++i) {
        node->boundingBox = AABB::Union(node->boundingBox, buildData[i].boundingBox);
    }
    
    uint32_t primitivesCount = end - start;
    
    if (primitivesCount < 10) {
        // Create leaf node
        node->primitivesOffset = orderedPrimitives.size();
        node->primitivesCount = primitivesCount;
        for (uint32_t i = start; i < end; ++i) {
            orderedPrimitives.push_back(_primitives[buildData[i].primitiveIndex]);
        }
        return node;
    } else {
        // Compute bounding box of primitive centroids used to choose split dimension
        AABB centroidsBB;
        
        for (uint32_t i = start; i < end; ++i) {
            centroidsBB = AABB::Union(centroidsBB, buildData[i].centroid);
        }
        uint32_t splitDimension = centroidsBB.getMaxDimension();
        
        // If bounding box extent is null, create a leaf node
        if (centroidsBB.min[splitDimension] == centroidsBB.max[splitDimension]) {
            node->primitivesOffset = orderedPrimitives.size();
            node->primitivesCount = primitivesCount;
            for (uint32_t i = start; i < end; ++i) {
                orderedPrimitives.push_back(_primitives[buildData[i].primitiveIndex]);
            }
            return node;
        }
        
        // Partitions primitives base on splitMethod
        uint32_t mid = (start + end)/2;
        switch (_splitMethod) {
            case SplitMiddle: {
                // Partition primitives through node's bbox middle
                float middle = 0.5f * (centroidsBB.min[splitDimension]
                                       + centroidsBB.max[splitDimension]);
                BuildPrimitiveInfo* midPtr = std::partition(&buildData[start],
                                                            &buildData[end],
                                                            CompareToPoint(splitDimension, middle));
                mid = midPtr - &buildData[0];
                if (mid != start && mid != end) {
                    break;
                }
            }
            case SplitEqualCounts: default: {
                mid = (start + end)/2;
                std::nth_element(&buildData[start],
                                 &buildData[mid],
                                 &buildData[end],
                                 ComparePoints(splitDimension));
                break;
            }
        }
        // Init interior node
        node->splitDimension = splitDimension;
        node->children[0] = recursiveBuild(buildData, start, mid, orderedPrimitives);
        node->children[1] = recursiveBuild(buildData, mid, end, orderedPrimitives);
        node->primitivesCount = 0;
    }
    return node;
}