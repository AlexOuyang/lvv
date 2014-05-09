//
//  AnimatedTransform.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/7/14.
//
//

#ifndef __CSE168_Rendering__AnimatedTransform__
#define __CSE168_Rendering__AnimatedTransform__

#include "Core/Transform.h"

class AnimatedTransform {
public:
    
    AnimatedTransform();
    ~AnimatedTransform();
    
    void setTransform(const Transform& t);
    void setTransforms(const Transform& t1, const Transform& t2);
    bool isActuallyAnimated() const;
    
    const Transform& operator[](int i) const;
    
    Transform interpolate(float time) const;
    
    AABB motionBounds(const AABB& box, bool useInverse) const;
    
    static void decompose(const mat4x4& m, vec3* translation, quat* rotation, mat4x4* scale);
    
private:
    bool        _actuallyAnimated;
    Transform   _transforms[2];
    vec3        _translations[2];
    quat        _rotations[2];
    mat4x4      _scales[2];
};

#endif /* defined(__CSE168_Rendering__AnimatedTransform__) */
