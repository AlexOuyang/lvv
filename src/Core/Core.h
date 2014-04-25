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

using namespace glm;

#include <QDebug>

class Ray;
class Intersection;
class AABB;
class Scene;
class Renderer;
class Material;
class Primitive;

namespace Core {
    const float Epsilon = 0.0001f;
    
    int roundUpPow2(int n);
}

#endif
