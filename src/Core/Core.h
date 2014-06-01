//
//  Core.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef CSE168_Rendering_Core_h
#define CSE168_Rendering_Core_h

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace glm;

#include <memory>
#include <rapidjson/document.h>
#include <iostream>

#include <QDebug>

class Spectrum;
class Ray;
class Intersection;
class AABB;
class Scene;
class Camera;
class Renderer;
class Material;
class Primitive;
class Transform;

class Core {
public:
    static const float Epsilon;
    
    static int roundUpPow2(int n);
    
    static std::string baseDirectory;
    static void setBaseDirectory(const std::string& dir);
};

/*
 * Debug utilities
 */

QDebug operator<<(QDebug dbg, const std::string& s);
QDebug operator<<(QDebug dbg, const vec2& v);
QDebug operator<<(QDebug dbg, const vec3& v);
QDebug operator<<(QDebug dbg, const vec4& v);
QDebug operator<<(QDebug dbg, const mat4x4& m);
QDebug operator<<(QDebug dbg, const quat& q);

#endif
