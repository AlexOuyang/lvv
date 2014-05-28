//
//  AreaLight.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#ifndef __CSE168_Rendering__AreaLight__
#define __CSE168_Rendering__AreaLight__

#include "Core/Core.h"
#include "Core/Light.h"
#include "Shapes/Mesh.h"
#include "Core/Transform.h"

class AreaLight : public Light {
public:
    
    static AreaLight* CreateFromMesh(Mesh* mesh, const Transform& t=Transform(),
                                     bool inverseNormal=false, int indexOffset=0);
    
    AreaLight();
    virtual ~AreaLight();
    
    void setPoints(const vec3& p1, const vec3& p2, const vec3& p3);
    void setNormal(const vec3& normal);
    void setIntensity(float intensity);
    void setColor(const vec3& color);
    void setColor(const std::shared_ptr<Texture>& texture);
    
    virtual Spectrum le(const Ray& ray, const Intersection* intersection=nullptr) const;
    virtual Spectrum sampleL(const vec3& point, float rayEpsilon,
                             const LightSample& lightSample,
                             vec3* wi, VisibilityTester* vt) const;
    
private:
    vec3                        _points[3];
    vec3                        _normal;
    float                       _intensity;
    std::shared_ptr<Texture>    _color;
};

#endif /* defined(__CSE168_Rendering__AreaLight__) */
