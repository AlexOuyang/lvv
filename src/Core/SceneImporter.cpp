//
//  SceneImporter.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/27/14.
//
//

#include "Importers/FBXImporter.h"
#include "Importers/AssimpImporter.h"
#include "Materials/Matte.h"
#include "Materials/AshikhminMaterial.h"
#include "Accelerators/BVHAccelerator.h"
#include "Lights/AreaLight.h"
#include "Lights/Skylight.h"

#include "SceneImporter.h"

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
    } else if (type == "fbx") {
        importer = FBXImporter::Load(value);
    } else {
        std::cerr << "SceneImporter error: unknown importer \"" << type << "\"" << std::endl;
        return std::shared_ptr<SceneImporter>();
    }
    
    std::string filename = value["filename"].GetString();
    if (filename[0] != '/') {
        filename = Core::baseDirectory + filename;
    }
    importer->setFilename(filename);
    
    // Load scene overrides
    if (value.HasMember("overrides")) {
        const rapidjson::Value& overridesValue = value["overrides"];
        if (!overridesValue.IsObject()) {
            std::cerr << "SceneImporter error: overrides must be specified as an object" << std::endl;
            return std::shared_ptr<SceneImporter>();
        }
        // Load material overrides
        if (overridesValue.HasMember("materials")) {
            const rapidjson::Value& materialsOverridesValue = overridesValue["materials"];
            if (!materialsOverridesValue.IsArray()) {
                std::cerr << "SceneImporter error: material overrides must be specified as an array" << std::endl;
                return std::shared_ptr<SceneImporter>();
            }
            for (int i = 0, l = materialsOverridesValue.Size(); i < l; ++i) {
                importer->addOverride(MaterialOverride::Load(materialsOverridesValue[i]));
            }
        }
        // Load primitives overrides
        if (overridesValue.HasMember("primitives")) {
            const rapidjson::Value& primitivesOverridesValue = overridesValue["primitives"];
            if (!primitivesOverridesValue.IsArray()) {
                std::cerr << "SceneImporter error: primitives overrides must be specified as an array" << std::endl;
                return std::shared_ptr<SceneImporter>();
            }
            for (int i = 0, l = primitivesOverridesValue.Size(); i < l; ++i) {
                importer->addOverride(PrimitiveOverride::Load(primitivesOverridesValue[i]));
            }
        }
        // Load lights overrides
        if (overridesValue.HasMember("lights")) {
            const rapidjson::Value& lightsOverridesValue = overridesValue["lights"];
            if (!lightsOverridesValue.IsArray()) {
                std::cerr << "SceneImporter error: light overrides must be specified as an array" << std::endl;
                return std::shared_ptr<SceneImporter>();
            }
            for (int i = 0, l = lightsOverridesValue.Size(); i < l; ++i) {
                importer->addOverride(LightOverride::Load(lightsOverridesValue[i]));
            }
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
    if (pattern[pattern.length()-1] == '*') {
        if (pattern[0] == '*') {
            return name.find(pattern.substr(1, pattern.length()-2)) != std::string::npos;
        }
        return name.find(pattern.substr(0, pattern.length()-1)) == 0;
    }
    return pattern == name;
}

vec3 SceneImporter::LoadVec3(const rapidjson::Value& value) {
    if (value.IsArray() && value.Size() == 3) {
        return vec3((float)value[0u].GetDouble(),
                    (float)value[1u].GetDouble(),
                    (float)value[2u].GetDouble());
    }
    return vec3();
}

SceneImporter::MaterialOverride SceneImporter::MaterialOverride::Load(const rapidjson::Value& value) {
    SceneImporter::MaterialOverride override;
    
    if (value.HasMember("name")) {
        override.namePattern = value["name"].GetString();
    }
    if (value.HasMember("material")) {
        override.sceneMaterial = value["material"].GetString();
    }
    if (value.HasMember("inherits")) {
        const rapidjson::Value& inheritedValues = value["inherits"];
        if (inheritedValues.IsArray()) {
            override.inheritedAttrs.isSet = true;
            for (uint_t i = 0, l = inheritedValues.Size(); i < l; ++i) {
                override.inheritedAttrs.value.push_back(inheritedValues[i].GetString());
            }
        }
    }
    return override;
}

SceneImporter::PrimitiveLightOverride SceneImporter::PrimitiveLightOverride::Load(const rapidjson::Value& value) {
    SceneImporter::PrimitiveLightOverride override;
    
    if (!value.HasMember("type")) {
        std::cerr << "PrimitiveLightOverride error: no type specified" << std::endl;
        return override;
    }
    
    std::string type = value["type"].GetString();
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    
    if (type == "area") {
        override.type = Area;
        if (value.HasMember("inverseNormal")) {
            override.inverseNormal = value["inverseNormal"].GetBool();
        }
        if (value.HasMember("indexOffset")) {
            override.indexOffset = value["indexOffset"].GetInt();
        }
        if (value.HasMember("isDirectional")) {
            override.isDirectional = value["isDirectional"].GetBool();
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
    
    if (value.HasMember("rotationOffset")) {
        override.rotationOffset.isSet = true;
        override.rotationOffset.value = value["rotationOffset"].GetDouble();
    }
    
    return override;
}

SceneImporter::PrimitiveVolumeOverride SceneImporter::PrimitiveVolumeOverride::Load(const rapidjson::Value& value) {
    SceneImporter::PrimitiveVolumeOverride override;
    
    if (!value.HasMember("type")) {
        std::cerr << "PrimitiveVolumeOverride error: no type specified" << std::endl;
        return override;
    }
    
    std::string type = value["type"].GetString();
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    
    if (type == "homogeneous") {
        override.type = Homogeneous;
    } else if (type == "grid") {
        override.type = Grid;
        
        if (value.HasMember("data")) {
            override.gridDataFile = value["data"].GetString();
        }
    }
    
    if (value.HasMember("sigmaA")) {
        override.sigmaA = LoadVec3(value["sigmaA"]);
    }
    if (value.HasMember("sigmaS")) {
        override.sigmaS = LoadVec3(value["sigmaS"]);
    }
    if (value.HasMember("le")) {
        override.le = LoadVec3(value["le"]);
    }
    if (value.HasMember("g")) {
        override.g = value["g"].GetDouble();
    }
    
    if (value.HasMember("stepSize")) {
        override.stepSize = value["stepSize"].GetDouble();
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
    if (value.HasMember("volume")) {
        override.volume.isSet = true;
        override.volume.value = PrimitiveVolumeOverride::Load(value["volume"]);
    }
    
    return override;
}

SceneImporter::LightOverride SceneImporter::LightOverride::Load(const rapidjson::Value& value) {
    SceneImporter::LightOverride override;
    
    if (value.HasMember("name")) {
        override.namePattern = value["name"].GetString();
    }
    if (value.HasMember("intensity")) {
        override.intensity.isSet = true;
        override.intensity.value = value["intensity"].GetDouble();
    }
    if (value.HasMember("direction")) {
        const rapidjson::Value& directionValue = value["direction"];
        if (directionValue.IsArray() && directionValue.Size() == 3) {
            override.direction.isSet = true;
            override.direction.value = LoadVec3(directionValue);
        }
    }
    if (value.HasMember("position")) {
        const rapidjson::Value& positionValue = value["position"];
        if (positionValue.IsArray() && positionValue.Size() == 3) {
            override.position.isSet = true;
            override.position.value = LoadVec3(positionValue);
        }
    }
    if (value.HasMember("color")) {
        override.color = Texture::Load(value["color"]);
    }
    
    return override;
}

SceneImporter::SceneImporter() :
_filename(), _meshAccelerationStructure(BVHAccelerationStructure),
_materialsOverrides(), _primitivesOverrides(), _lightsOverrides() {
    
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
    _materialsOverrides.push_back(override);
}

void SceneImporter::addOverride(const SceneImporter::PrimitiveOverride& override) {
    _primitivesOverrides.push_back(override);
}

void SceneImporter::addOverride(const SceneImporter::LightOverride& override) {
    _lightsOverrides.push_back(override);
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
    for (const MaterialOverride& override : _materialsOverrides) {
        if (MatchName(override.namePattern, attrs.name)) {
            // Look for the overriden material in the scene
            std::shared_ptr<Material> material = scene.getMaterial(override.sceneMaterial);
            if (material) {
                // Clone material
                material = material->clone();
                material->setName(material->getName()+"_inherits_"+attrs.name);
                if (override.inheritedAttrs.isSet) {
                    for (const std::string& attr : override.inheritedAttrs.value) {
                        if (attr == "diffuseIntensity") {
                            if (attrs.diffuseIntensityTexture) {
                                material->setDiffuseIntensity(attrs.diffuseIntensityTexture);
                            } else {
                                material->setDiffuseIntensity(attrs.diffuseIntensity);
                            }
                        }
                        if (attr == "specularIntensity") {
                            if (attrs.specularIntensityTexture) {
                                material->setSpecularIntensity(attrs.specularIntensityTexture);
                            } else {
                                material->setSpecularIntensity(1.f - attrs.diffuseIntensity);
                            }
                        }
                        
                        if (attr == "diffuseColor") {
                            if (attrs.diffuseTexture) {
                                material->setDiffuseColor(attrs.diffuseTexture);
                            } else {
                                material->setDiffuseColor(attrs.diffuseColor);
                            }
                        }
                        if (attr == "specularColor") {
                            if (attrs.specularTexture) {
                                material->setSpecularColor(attrs.specularTexture);
                            } else {
                                material->setSpecularColor(attrs.specularColor);
                            }
                        }
                        
                        if (attr == "normalMap") {
                            if (attrs.normalMap) {
                                material->setNormalMap(attrs.normalMap);
                            }
                        }
                        
                        if (attr == "alphaTexture") {
                            if (attrs.alphaTexture) {
                                material->setAlphaTexture(attrs.alphaTexture);
                            }
                        }
                    }
                }
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
        if (attrs.shadingMode == ImportedMaterialAttributes::Lambert) {
            std::shared_ptr<Matte> matte = std::make_shared<Matte>();
            if (attrs.diffuseTexture) {
                matte->setDiffuseColor(attrs.diffuseTexture);
            } else {
                matte->setDiffuseColor(attrs.diffuseColor);
            }
            
            material = matte;
        } else if (attrs.shadingMode == ImportedMaterialAttributes::Phong) {
            std::shared_ptr<AshikhminMaterial> mtl = std::make_shared<AshikhminMaterial>();
            
            if (attrs.diffuseIntensityTexture) {
                mtl->setDiffuseIntensity(attrs.diffuseIntensityTexture);
            } else {
                mtl->setDiffuseIntensity(attrs.diffuseIntensity);
            }
            
            if (attrs.diffuseTexture) {
                mtl->setDiffuseColor(attrs.diffuseTexture);
            } else {
                mtl->setDiffuseColor(attrs.diffuseColor);
            }
            
            if (attrs.specularIntensityTexture) {
                mtl->setSpecularIntensity(attrs.specularIntensityTexture);
            } else {
                mtl->setSpecularIntensity(1.f - attrs.diffuseIntensity);
            }
            
            if (attrs.specularTexture) {
                mtl->setSpecularColor(attrs.specularTexture);
            } else {
                mtl->setSpecularColor(attrs.specularColor);
            }
            
            material = mtl;
        } else {
            std::cerr << "SceneImporter error: unkwnown material shading mode" << std::endl;
            material = std::make_shared<Matte>();
        }
        
        if (attrs.normalMap) {
            material->setNormalMap(attrs.normalMap);
        }
        
        if (attrs.alphaTexture) {
            material->setAlphaTexture(attrs.alphaTexture);
        }
        
        material->setName(attrs.name);
    }
    
    scene.addMaterial(material);
    return material;
}

bool SceneImporter::applyPrimitiveOverrides(Scene& scene, const std::string& name,
                                            const std::shared_ptr<TransformedPrimitive>& transformedPrimitive,
                                            GeometricPrimitive& p,
                                            const ImportedMaterialAttributes* material,
                                            MeshBase* mesh) const {
    const Transform& transform = transformedPrimitive->getTransform();
    
    // Look for a matching override
    for (const PrimitiveOverride& override : _primitivesOverrides) {
        if (MatchName(override.namePattern, name)) {
            // Apply material overrides
            if (override.material.isSet) {
                std::shared_ptr<Material> material = scene.getMaterial(override.material.value);
                if (material) {
                    p.setMaterial(material);
                } else {
                    std::cerr << "SceneImporter Error: Overriden material \"" <<
                    override.material.value << "\" not found" << std::endl;
                }
            }
            // Apply light overrides
            if (override.light.isSet) {
                const PrimitiveLightOverride& lightOverride = override.light.value;
                if (lightOverride.type == PrimitiveLightOverride::Area && mesh) {
                    // Create area light
                    AreaLight* light = AreaLight::CreateFromMesh(mesh, transform,
                                                                 lightOverride.inverseNormal,
                                                                 lightOverride.indexOffset);
                    
                    if (!light) {
                        continue;
                    }
                    
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
                    
                    if (lightOverride.isDirectional) {
                        light->setDirectional(true);
                        // Do not add mesh to scene
                        return false;
                    }
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
                    
                    if (lightOverride.intensity.isSet) {
                        light->setIntensity(lightOverride.intensity.value);
                    }
                    
                    Transform t = Transform::Inverse(transform);
                    if (lightOverride.rotationOffset.isSet) {
                        t.rotate(glm::radians(lightOverride.rotationOffset.value), vec3(0.f, 1.f, 0.f));
                    }
                    
                    light->setTransform(t);
                    
                    // Add light to scene
                    scene << light;
                    
                    // Return false so the original primitive is not added to the scene
                    return false;
                }
            }
            // Apply volume overrides
            if (override.volume.isSet) {
                const PrimitiveVolumeOverride& volumeOverride = override.volume.value;

                AABB bounds = transform(p.getBoundingBox());
                
                // Create volume base on type
                Volume* volume = nullptr;
                
                if (volumeOverride.type == PrimitiveVolumeOverride::Homogeneous) {
                    HomogeneousVolume* homogeneous = new HomogeneousVolume();
                    
                    homogeneous->setBounds(bounds);
                    homogeneous->setSigmaA(Spectrum(volumeOverride.sigmaA));
                    homogeneous->setSigmaS(Spectrum(volumeOverride.sigmaS));
                    homogeneous->setLe(Spectrum(volumeOverride.le));
                    homogeneous->setPhaseParameter(volumeOverride.g);
                    homogeneous->setStepSize(volumeOverride.stepSize);
                    
                    volume = homogeneous;
                } else if (volumeOverride.type == PrimitiveVolumeOverride::Grid) {
                    GridVolume* grid = new GridVolume();
                    
                    if (!grid->loadData(volumeOverride.gridDataFile)) {
                        delete grid;
                        grid = nullptr;
                    } else {
                        grid->setParentNode(transformedPrimitive);
                        grid->setBounds(p.getBoundingBox());
                        grid->setSigmaA(Spectrum(volumeOverride.sigmaA));
                        grid->setSigmaS(Spectrum(volumeOverride.sigmaS));
                        grid->setLe(Spectrum(volumeOverride.le));
                        grid->setPhaseParameter(volumeOverride.g);
                        grid->setStepSize(volumeOverride.stepSize);
                    }
                    
                    // Create animation evaluator
                    if (grid) {
                        scene.registerAnimationEvaluator(std::make_shared<GridVolumeAnimationEvaluator>(grid));
                    }
                    
                    volume = grid;
                }
                
                if (!volume) {
                    continue;
                }
                
                scene << volume;
                
                // Return false so the original primitive is not added to the scene
                return false;
            }
            return true;
        }
    }
    return true;
}

bool SceneImporter::applyLightOverrides(Scene&, Light* light) const {
    // Look for a matching override
    for (const LightOverride& override : _lightsOverrides) {
        if (MatchName(override.namePattern, light->getName())) {
            DirectionalLight* directional = dynamic_cast<DirectionalLight*>(light);
            if (directional) {
                if (override.intensity.isSet) {
                    directional->setIntensity(override.intensity.value);
                }
                if (override.direction.isSet) {
                    directional->setDirection(override.direction.value);
                }
                if (override.color) {
                    directional->setSpectrum(Spectrum(override.color->evaluateVec3(vec2())));
                }
                return true;
            }
            PointLight* point = dynamic_cast<PointLight*>(light);
            if (point) {
                if (override.intensity.isSet) {
                    point->setIntensity(override.intensity.value);
                }
                if (override.direction.isSet) {
                    point->setPosition(override.direction.value);
                }
                if (override.color) {
                    point->setSpectrum(Spectrum(override.color->evaluateVec3(vec2())));
                }
                return true;
            }
        }
    }
    return true;
}

SceneImporter::GridVolumeAnimationEvaluator::GridVolumeAnimationEvaluator(GridVolume* vol) : volume(vol) {
    
}

SceneImporter::GridVolumeAnimationEvaluator::~GridVolumeAnimationEvaluator() {
    
}

void SceneImporter::GridVolumeAnimationEvaluator::evaluate(float tstart, float tend) {
    volume->setFrame((int)tend);
}