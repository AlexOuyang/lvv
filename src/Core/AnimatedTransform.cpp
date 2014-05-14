//
//  AnimatedTransform.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/7/14.
//
//

#include "AnimatedTransform.h"

#include "Ray.h"

AnimatedTransform::AnimatedTransform() :
_actuallyAnimated(false), _transforms(),
_translations(), _rotations(), _scales() {
    
}

AnimatedTransform::~AnimatedTransform() {
    
}

void AnimatedTransform::setTransform(const Transform& t) {
    _actuallyAnimated = false;
    _transforms[0] = t;
}

void AnimatedTransform::setTransforms(const Transform& t1, const Transform& t2) {
    _actuallyAnimated = true;
    _transforms[0] = t1;
    _transforms[1] = t2;
    // Decompose matrices
    decompose(t1.getMatrix(), &_translations[0], &_rotations[0], &_scales[0]);
    decompose(t2.getMatrix(), &_translations[1], &_rotations[1], &_scales[1]);
    if (dot(_rotations[0], _rotations[1]) < 0.f) {
        _rotations[1] = -_rotations[1];
    }
}

bool AnimatedTransform::isActuallyAnimated() const {
    return _actuallyAnimated;
}

const Transform& AnimatedTransform::operator[](int i) const {
    return _transforms[i];
}

Transform AnimatedTransform::interpolate(float time) const {
    if (!_actuallyAnimated || time <= 0.f) {
        return _transforms[0];
    }
    if (time >= 1.f) {
        return _transforms[1];
    }
    
    // Interpolate translation
    vec3 t = mix(_translations[0], _translations[1], time);
    
    // Interpolate rotation
    quat r = mix(_rotations[0], _rotations[1], time);
    
    // Interpolate scale
    mat4x4 s = mix(_scales[0], _scales[1], time);
    
    // Recompose interpolated matrix
    mat4x4 m;
    m = glm::translate(m, t) * mat4_cast(r) * s;
    return m;
}

AABB AnimatedTransform::motionBounds(const AABB& box, bool useInverse) const {
    if (!_actuallyAnimated) {
        return useInverse ? Transform::Inverse(_transforms[0])(box) : _transforms[0](box);
    }
    
    AABB bounds;
    
    const int steps = 128;
    for (int i = 0; i < steps; ++i) {
        Transform t = interpolate((float)i/(steps-1));
        if (useInverse) {
            t = Transform::Inverse(t);
        }
        bounds = AABB::Union(bounds, t(box));
    }
    return bounds;
}

void AnimatedTransform::decompose(const mat4x4& m, vec3* translation, quat* rotation,
                                  mat4x4* scale) {
    // Decompose function from Pbrt
    // https://github.com/mmp/pbrt-v2/blob/master/src/core/transform.cpp
    
    // Extract translation from transformation matrix
    translation->x = m[3][0];
    translation->y = m[3][1];
    translation->z = m[3][2];
    
    // Compute new transformation matrix M without translation
    mat4x4 M = m;
    for (int i = 0; i < 3; ++i)
        M[i][3] = M[3][i] = 0.f;
    M[3][3] = 1.f;
    
    // Extract rotation from transformation matrix
    float norm;
    int count = 0;
    mat4x4 R = M;
    do {
        // Compute next matrix Rnext in series
        mat4x4 Rnext;
        mat4x4 Rit = inverse(transpose(R));
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                Rnext[i][j] = 0.5f * (R[i][j] + Rit[i][j]);
        
        // Compute norm of difference between R and Rnext
        norm = 0.f;
        for (int i = 0; i < 3; ++i) {
            float n = (fabsf(R[i][0] - Rnext[i][0]) +
                       fabsf(R[i][1] - Rnext[i][1]) +
                       fabsf(R[i][2] - Rnext[i][2])
                       );
            norm = max(norm, n);
        }
        R = Rnext;
    } while (++count < 100 && norm > .0001f);
    
    *rotation = quat_cast(R);
    
    // Compute scale _S_ using rotation and original matrix
    *scale = inverse(R) * M;
}
