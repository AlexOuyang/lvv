//
//  SceneImporter.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/27/14.
//
//

#include "SceneImporter.h"

#include "Importers/AssimpImporter.h"
#include "Materials/Matte.h"
#include "Accelerators/BVHAccelerator.h"

std::shared_ptr<SceneImporter> SceneImporter::Load(const rapidjson::Value& value) {
    // Check if mandatory values are specified
    if (!value.HasMember("importer")) {
        std::cerr << "SceneImporter error: no importer specified" << std::endl;
        return std::shared_ptr<SceneImporter>();
    }
    if (!value.HasMember("filename")) {
        std::cerr << "SceneImporter error: no filename specified" << std::endl;
        return std::shared_ptr<SceneImporter>();
    }
    
    // Create importer based on given type
    std::string type = value["importer"].GetString();
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    
    std::shared_ptr<SceneImporter> importer;
    if (type == "assimp") {
        importer = AssimpImporter::Load(value);
    } else {
        std::cerr << "SceneImporter error: unknown importer \"" << type << "\"" << std::endl;
        return std::shared_ptr<SceneImporter>();
    }
    
    importer->setFilename(value["filename"].GetString());
    
    // Load scene overrides
    if (value.HasMember("overrides")) {
        const rapidjson::Value& overridesValue = value["overrides"];
        if (!overridesValue.IsObject()) {
            std::cerr << "SceneImporter error: overrides must be specified as an object" << std::endl;
            return std::shared_ptr<SceneImporter>();
        }
        const rapidjson::Value& primitivesOverridesValue = overridesValue["primitives"];
        if (!primitivesOverridesValue.IsArray()) {
            std::cerr << "SceneImporter error: primitives overrides must be specified as an array" << std::endl;
            return std::shared_ptr<SceneImporter>();
        }
        for (int i = 0, l = primitivesOverridesValue.Size(); i < l; ++i) {
            importer->addOverride(PrimitiveOverride::Load(primitivesOverridesValue[i]));
        }
    }

    if (value.HasMember("meshAccelerationStructure")) {
        std::string str = value["meshAccelerationStructure"].GetString();
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        if (str == "bvh") {
            importer->setMeshAccelerationStructure(BVHAccelerationStructure);
        } else {
            std::cerr << "SceneImporter warning: unknown \"" << str << "\" acceleration structure"
            << std::endl;
        }
    }
    
    return importer;
}

bool SceneImporter::MatchName(const std::string& name1, const std::string& name2) {
    return name1 == name2;
}

SceneImporter::AreaLightOverride SceneImporter::AreaLightOverride::Load(const rapidjson::Value& value) {
    SceneImporter::AreaLightOverride override;
    
    if (value.HasMember("inverseNormal")) {
        override.inverseNormal = value["inverseNormal"].GetBool();
    }
    if (value.HasMember("indexOffset")) {
        override.indexOffset = value["indexOffset"].GetInt();
    }
    if (value.HasMember("color")) {
        std::shared_ptr<Texture> texture = Texture::Load(value["color"]);
        if (texture) {
            override.color = texture;
        }
    }
    if (value.HasMember("intensity")) {
        override.intensity = value["intensity"].GetDouble();
    }
    
    return override;
}

SceneImporter::PrimitiveOverride SceneImporter::PrimitiveOverride::Load(const rapidjson::Value& value) {
    SceneImporter::PrimitiveOverride override;
    
    if (value.HasMember("name")) {
        override.namePattern = value["name"].GetString();
    }
    if (value.HasMember("material")) {
        override.hasMaterial = true;
        override.material = value["material"].GetString();
    } else {
        override.hasMaterial = false;
    }
    if (value.HasMember("areaLight")) {
        override.hasAreaLight = true;
        override.areaLight = AreaLightOverride::Load(value["areaLight"]);
    } else {
        override.hasAreaLight = false;
    }
    return override;
}

SceneImporter::SceneImporter() :
_filename(), _meshAccelerationStructure(BVHAccelerationStructure),
_materialOverrides(), _primitivesOverrides() {
    
}

SceneImporter::~SceneImporter() {
    
}

void SceneImporter::setFilename(const std::string& filename) {
    _filename = filename;
}

void SceneImporter::setMeshAccelerationStructure(MeshAccelerationStructure s) {
    _meshAccelerationStructure = s;
}

void SceneImporter::addOverride(const SceneImporter::MaterialOverride& override) {
    _materialOverrides.push_back(override);
}

void SceneImporter::addOverride(const SceneImporter::PrimitiveOverride& override) {
    _primitivesOverrides.push_back(override);
}

std::shared_ptr<Aggregate> SceneImporter::createMeshAccelerationStructure() const {
    if (_meshAccelerationStructure == BVHAccelerationStructure) {
        return std::make_shared<BVHAccelerator>();
    }
    return std::shared_ptr<Aggregate>();
}

std::shared_ptr<Material> SceneImporter::getOverridenMaterial(const ImportedMaterialAttributes& attrs,
                                                              const Scene& scene) const {
    // Look for a matching override
    for (const MaterialOverride& override : _materialOverrides) {
        if (MatchName(override.namePattern, attrs.name)) {
            // Look for the overriden material in the scene
            std::shared_ptr<Material> material = scene.getMaterial(override.sceneMaterial);
            if (material) {
                // TODO: handle overriden properties
                return material;
            }
        }
    }
    
    return std::shared_ptr<Material>();
}

std::shared_ptr<Material> SceneImporter::addImportedMaterial(const SceneImporter::ImportedMaterialAttributes &attrs,
                                        Scene &scene) {
    std::shared_ptr<Material> material = getOverridenMaterial(attrs, scene);
    
    if (!material) {
        std::shared_ptr<Matte> defaultMtl = std::make_shared<Matte>();
        if (!attrs.diffuseTexturePath.empty()) {
            std::shared_ptr<Texture> t = ImageLoading::LoadImage(attrs.diffuseTexturePath);
            if (!t) {
                std::cerr << "SceneImporter Error: couldn't load texture "
                << attrs.diffuseTexturePath << std::endl;
            } else {
                defaultMtl->setColor(t);
            }
        } else {
            defaultMtl->setColor(attrs.diffuseColor);
        }
        material = defaultMtl;
    }
    
    scene.addMaterial(material);
    return material;
}

void SceneImporter::applyPrimitiveOverrides(Scene& scene, const std::string& name,
                                            const Transform& transform, GeometricPrimitive& p,
                                            Mesh* mesh) const {
    // Look for a matching override
    for (const PrimitiveOverride& override : _primitivesOverrides) {
        if (MatchName(override.namePattern, name)) {
            if (override.hasMaterial) {
                std::shared_ptr<Material> material = scene.getMaterial(override.material);
                if (material) {
                    p.setMaterial(material);
                } else {
                    std::cerr << "SceneImporter Error: Overriden material \"" <<
                    override.material << "\" not found" << std::endl;
                }
            }
            if (override.hasAreaLight) {
                // Create area light
                AreaLight* light = AreaLight::CreateFromMesh(mesh, transform,
                                                             override.areaLight.inverseNormal,
                                                             override.areaLight.indexOffset);
                light->setColor(override.areaLight.color);
                light->setIntensity(override.areaLight.intensity);
                
                // Add light to geometric primitive
                p.setAreaLight(light);
                
                // Add light to scene
                scene << light;
            }
            return;
        }
    }
}