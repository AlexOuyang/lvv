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
#include "Lights/AreaLight.h"
#include "Lights/Skylight.h"

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

bool SceneImporter::MatchName(const std::string& pattern, const std::string& name) {
    return pattern.find(name) != std::string::npos;
}

SceneImporter::PrimitiveLightOverride SceneImporter::PrimitiveLightOverride::Load(const rapidjson::Value& value) {
    SceneImporter::PrimitiveLightOverride override;
    
    if (!value.HasMember("type")) {
        std::cerr << "PrimitiveLightOverride error: no type specified" << std::endl;
        return override;
    }
    
    const std::string type = value["type"].GetString();
    if (type == "area") {
        override.type = Area;
        if (value.HasMember("inverseNormal")) {
            override.inverseNormal = value["inverseNormal"].GetBool();
        }
        if (value.HasMember("indexOffset")) {
            override.indexOffset = value["indexOffset"].GetInt();
        }
    } else if (type == "environment") {
        override.type = Environment;
    }
    if (value.HasMember("color")) {
        std::shared_ptr<Texture> texture = Texture::Load(value["color"]);
        if (texture) {
            override.color = texture;
        }
    }
    if (value.HasMember("intensity")) {
        override.intensity.isSet = true;
        override.intensity.value = value["intensity"].GetDouble();
    }
    
    return override;
}

SceneImporter::PrimitiveOverride SceneImporter::PrimitiveOverride::Load(const rapidjson::Value& value) {
    SceneImporter::PrimitiveOverride override;
    
    if (value.HasMember("name")) {
        override.namePattern = value["name"].GetString();
    }
    if (value.HasMember("material")) {
        override.material.isSet = true;
        override.material.value = value["material"].GetString();
    }
    if (value.HasMember("light")) {
        override.light.isSet = true;
        override.light.value = PrimitiveLightOverride::Load(value["light"]);
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
        if (attrs.diffuseTexture) {
            defaultMtl->setColor(attrs.diffuseTexture);
        } else {
            defaultMtl->setColor(attrs.diffuseColor);
        }

        material = defaultMtl;
    }
    
    scene.addMaterial(material);
    return material;
}

bool SceneImporter::applyPrimitiveOverrides(Scene& scene, const std::string& name,
                                            const Transform& transform, GeometricPrimitive& p,
                                            const ImportedMaterialAttributes* material,
                                            Mesh* mesh) const {
    // Look for a matching override
    for (const PrimitiveOverride& override : _primitivesOverrides) {
        if (MatchName(override.namePattern, name)) {
            if (override.material.isSet) {
                std::shared_ptr<Material> material = scene.getMaterial(override.material.value);
                if (material) {
                    p.setMaterial(material);
                } else {
                    std::cerr << "SceneImporter Error: Overriden material \"" <<
                    override.material.value << "\" not found" << std::endl;
                }
            }
            if (override.light.isSet) {
                const PrimitiveLightOverride& lightOverride = override.light.value;
                if (lightOverride.type == PrimitiveLightOverride::Area) {
                    // Create area light
                    AreaLight* light = AreaLight::CreateFromMesh(mesh, transform,
                                                                 lightOverride.inverseNormal,
                                                                 lightOverride.indexOffset);
                    
                    if (lightOverride.color) {
                        light->setColor(lightOverride.color);
                    } else if (material) {
                        if (material->diffuseTexture) {
                            light->setColor(material->diffuseTexture);
                        } else {
                            light->setColor(material->diffuseColor);
                        }
                    }
                    
                    if (lightOverride.intensity.isSet) {
                        light->setIntensity(lightOverride.intensity.value);
                    }
                    
                    // Add light to geometric primitive
                    p.setAreaLight(light);
                    
                    // Add light to scene
                    scene << light;
                } else if (lightOverride.type == PrimitiveLightOverride::Environment) {
                    // Create environment light
                    SkyLight* light = new SkyLight();
                    
                    if (lightOverride.color) {
                        light->setColor(lightOverride.color);
                    } else if (material) {
                        if (material->diffuseTexture) {
                            light->setColor(material->diffuseTexture);
                        } else {
                            light->setColor(material->diffuseColor);
                        }
                    }
                    
                    light->setTransform(transform);
                    
                    // Add light to scene
                    scene << light;
                    
                    // Return false so the original primitive is not added to the scene
                    return false;
                }
            }
            return true;
        }
    }
    return true;
}