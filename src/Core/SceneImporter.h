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

class SceneImporter {
public:
    
    static std::shared_ptr<SceneImporter> Load(const rapidjson::Value& value);
    static bool MatchName(const std::string& name1, const std::string& name2);
    
    struct AreaLightOverride {
        static AreaLightOverride Load(const rapidjson::Value& value);
        
        bool                        inverseNormal;
        int                         indexOffset;
        std::shared_ptr<Texture>    color;
        float                       intensity;
    };
    
    struct MaterialOverride {
        std::string namePattern;
        std::string sceneMaterial;
    };
    
    struct PrimitiveOverride {
        static PrimitiveOverride Load(const rapidjson::Value& value);
        
        std::string         namePattern;
        bool                hasMaterial;
        std::string         material;
        bool                hasAreaLight;
        AreaLightOverride   areaLight;
    };
    
    struct ImportedMaterialAttributes {
        std::string     name;
        vec3            diffuseColor;
        std::string     diffuseTexturePath;
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
    
    std::shared_ptr<Aggregate> createMeshAccelerationStructure() const;
    
    std::shared_ptr<Material> getOverridenMaterial(const ImportedMaterialAttributes& attrs, const Scene& scene) const;
    std::shared_ptr<Material> addImportedMaterial(const ImportedMaterialAttributes& attrs, Scene& scene);
    
    void applyPrimitiveOverrides(Scene& scene, const std::string& name,
                                 const Transform& transform, GeometricPrimitive& p,
                                 Mesh* mesh=nullptr) const;
    
protected:
    std::string                     _filename;
    MeshAccelerationStructure       _meshAccelerationStructure;
    std::vector<MaterialOverride>   _materialOverrides;
    std::vector<PrimitiveOverride>  _primitivesOverrides;
};

#endif /* defined(__CSE168_Rendering__SceneImporter__) */
