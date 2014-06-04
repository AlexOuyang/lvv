//
//  Transform.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/13/14.
//
//

#ifndef __CSE168_Rendering__Transform__
#define __CSE168_Rendering__Transform__

#include "Core/Core.h"

#include "AABB.h"

class Transform {
public:
    
    static Transform Inverse(const Transform& t);
    
    Transform();
    Transform(const mat4x4& m);
    
    void lookAt(const vec3& position, const vec3& target, const vec3& up=vec3(0, 1, 0));
    void setTranslation(const vec3& v);
    void translate(const vec3& v);
    void rotate(float angle, const vec3& axis);
    void scale(const vec3& scale);
    void applyMatrix(const mat4x4& matrix);
    
    vec3 applyToVector(const vec3& v) const;
    vec3 applyToNormal(const vec3& n) const;
    vec3 operator()(const vec3& p) const;
    Ray operator()(const Ray& r) const;
    AABB operator()(const AABB& box) const;
    Transform operator()(const Transform& t) const;
    
    mat4x4  getMatrix() const { return _matrix; }
    vec4    getMatrixColumn(int col) const { return _matrix[col]; }
    
private:
    mat4x4 _matrix;
};

#endif /* defined(__CSE168_Rendering__Transform__) */
