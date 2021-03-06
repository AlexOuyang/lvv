//
//  SceneImporter.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/27/14.
//
//

#ifndef __CSE168_Rendering__SceneImporter__
#define __CSE168_Rendering__SceneImporter__

#include <vector>

#include "Core.h"
#include "Aggregate.h"
#include "Texture.h"
#include "TransformedPrimitive.h"
#include "GeometricPrimitive.h"
#include "Shapes/Mesh.h"
#include "Volumes/HomogeneousVolume.h"
#include "Volumes/GridVolume.h"
#include "Core/AnimationEvaluator.h"

class SceneImporter {
public:
    
    static std::shared_ptr<SceneImporter> Load(const rapidjson::Value& value);
    static bool MatchName(const std::string& name1, const std::string& name2);
    static vec3 LoadVec3(const rapidjson::Value& value);
    
    template<class T> struct OverridenProperty {
        OverridenProperty() : isSet(false) {}
        
        bool    isSet;
        T       value;
    };
    
    struct MaterialOverride {
        static MaterialOverride Load(const rapidjson::Value& value);
        
        std::string                                 namePattern;
        std::string                                 sceneMaterial;
        OverridenProperty<std::vector<std::string>> inheritedAttrs;
    };
    
    struct PrimitiveLightOverride {
        enum Type {
            Area,
            Environment
        };
        
        static PrimitiveLightOverride Load(const rapidjson::Value& value);
        
        PrimitiveLightOverride()
        : type(), inverseNormal(false), indexOffset(), isDirectional(false),
        color(), intensity() {}
        
        Type                        type;
        bool                        inverseNormal;
        int                         indexOffset;
        bool                        isDirectional;
        std::shared_ptr<Texture>    color;
        OverridenProperty<float>    intensity;
        OverridenProperty<float>    rotationOffset;
    };
    
    struct PrimitiveVolumeOverride {
        enum Type {
            Homogeneous,
            Grid
        };
        
        static PrimitiveVolumeOverride Load(const rapidjson::Value& value);
        
        Type        type;
        vec3        sigmaA;
        vec3        sigmaS;
        vec3        le;
        float       g;
        std::string gridDataFile;
        float       stepSize;
    };
    
    struct PrimitiveOverride {
        static PrimitiveOverride Load(const rapidjson::Value& value);
        
        std::string                                 namePattern;
        OverridenProperty<std::string>              material;
        OverridenProperty<PrimitiveLightOverride>   light;
        OverridenProperty<PrimitiveVolumeOverride>  volume;
    };
    
    struct LightOverride {
        static LightOverride Load(const rapidjson::Value& value);
        
        std::string                 namePattern;
        OverridenProperty<float>    intensity;
        OverridenProperty<vec3>     direction;
        OverridenProperty<vec3>     position;
        std::shared_ptr<Texture>    color;
    };
    
    struct ImportedMaterialAttributes {
        enum ShadingMode {
            Lambert,
            Phong
        };
        
        std::string                 name;
        ShadingMode                 shadingMode;
        vec3                        diffuseColor;
        std::shared_ptr<Texture>    diffuseTexture;
        float                       diffuseIntensity;
        std::shared_ptr<Texture>    diffuseIntensityTexture;
        vec3                        specularColor;
        std::shared_ptr<Texture>    specularTexture;
        std::shared_ptr<Texture>    specularIntensityTexture;
        std::shared_ptr<Texture>    normalMap;
        std::shared_ptr<Texture>    alphaTexture;
    };
    
    enum MeshAccelerationStructure {
        BVHAccelerationStructure
    };
    
    SceneImporter();
    virtual ~SceneImporter();
    
    virtual bool import(Scene& scene) = 0;
    
    void setFilename(const std::string& filename);
    void setMeshAccelerationStructure(MeshAccelerationStructure s);
    void addOverride(const MaterialOverride& override);
    void addOverride(const PrimitiveOverride& override);
    void addOverride(const LightOverride& override);
    
    std::shared_ptr<Aggregate> createMeshAccelerationStructure() const;
    
    std::shared_ptr<Material> getOverridenMaterial(const ImportedMaterialAttributes& attrs, const Scene& scene) const;
    std::shared_ptr<Material> addImportedMaterial(const ImportedMaterialAttributes& attrs, Scene& scene);
    
    bool applyPrimitiveOverrides(Scene& scene, const std::string& name,
                                 const std::shared_ptr<TransformedPrimitive>& transformedPrimitive,
                                 GeometricPrimitive& p,
                                 const ImportedMaterialAttributes* material=nullptr,
                                 MeshBase* mesh=nullptr) const;
    bool applyLightOverrides(Scene& scene, Light* light) const;
    
protected:
    
    class GridVolumeAnimationEvaluator : public AnimationEvaluator {
    public:
        GridVolumeAnimationEvaluator(GridVolume* vol);
        virtual ~GridVolumeAnimationEvaluator();
        
        virtual void evaluate(float tstart, float tend);
        
        GridVolume* volume;
    };
    
    std::string                     _filename;
    MeshAccelerationStructure       _meshAccelerationStructure;
    std::vector<MaterialOverride>   _materialsOverrides;
    std::vector<PrimitiveOverride>  _primitivesOverrides;
    std::vector<LightOverride>      _lightsOverrides;
};

#endif /* defined(__CSE168_Rendering__SceneImporter__) */
