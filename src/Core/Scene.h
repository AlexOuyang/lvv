//
//  Scene.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef __CSE168_Rendering__Scene__
#define __CSE168_Rendering__Scene__

#include <vector>
#include <map>

#include "Core/Core.h"
#include "Aggregate.h"
#include "Volume.h"
#include "Light.h"
#include "Camera.h"

class Scene {
public:
    
    Scene(Aggregate* aggregate);
    ~Scene();
    
    bool intersect(const Ray& ray, Intersection* intersection) const;
    bool intersectP(const Ray& ray) const;
    
    void preprocess();
    
    void addLight(Light* light);
    void addPrimitive(const std::shared_ptr<Primitive>& primitive);
    void setVolume(Volume* volume);
    
    Scene& operator<<(Light* light);
    Scene& operator<<(const std::shared_ptr<Camera>& camera);
    Scene& operator<<(const std::shared_ptr<Primitive>& primitive);
    
    const std::vector<Light*>&      getLights() const;
    Volume*                         getVolume() const;
    
    void addMaterial(const std::shared_ptr<Material>& mtl);
    std::shared_ptr<Material> getMaterial(const std::string& name) const;
    
    void                    addCamera(const std::shared_ptr<Camera>& camera);
    std::shared_ptr<Camera> getCamera(const std::string& name="") const;
    bool                    setDefaultCamera(const std::string& name);
    
    static std::shared_ptr<Scene> Load(const rapidjson::Value& value);
    
private:
    std::vector<Light*>                                 _lights;
    Aggregate*                                          _aggregate;
    Volume*                                             _volume;
    std::map<std::string, std::shared_ptr<Material>>    _materials;
    std::map<std::string, std::shared_ptr<Camera>>      _cameras;
    std::string                                         _defaultCamera;
};

#endif /* defined(__CSE168_Rendering__Scene__) */