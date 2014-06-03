//
//  FBXImporter.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/1/14.
//
//

#include "FBXImporter.h"

std::shared_ptr<FBXImporter> FBXImporter::Load(const rapidjson::Value&) {
    // No configuration to load, just create an importer
    return std::make_shared<FBXImporter>();
}

FBXImporter::FBXImporter() : SceneImporter(), _fbxManager(nullptr) {
    _fbxManager = FbxManager::Create();
}

FBXImporter::~FBXImporter() {
    _fbxManager->Destroy();
}

bool FBXImporter::import(Scene &scene) {
    FbxImporter* fbxImporter = FbxImporter::Create(_fbxManager, "");
    
    if (!fbxImporter->Initialize(_filename.c_str())) {
        std::cerr << "FbxImporter error: " << fbxImporter->GetStatus().GetErrorString() << std::endl;
        return false;
    }
    
    FbxScene* fbxScene = FbxScene::Create(_fbxManager, "ImportedScene");
    
    if (!fbxImporter->Import(fbxScene)) {
        std::cerr << "FbxImporter error: " << fbxImporter->GetStatus().GetErrorString() << std::endl;
        return false;
    }
    
    FbxNode* rootNode = fbxScene->GetRootNode();
    if (!rootNode) {
        return false;
    }
    
    importNode(rootNode, scene);
    
    return true;
}

void FBXImporter::importNode(const FbxNode *node, Scene &scene) {
    // Import node attributes
    for (uint_t i = 0; i < node->GetNodeAttributeCount(); ++i) {
        importNodeAttribute(node, node->GetNodeAttributeByIndex(i), scene);
    }
    
    for (uint_t i = 0; i < node->GetChildCount(); ++i) {
        importNode(node->GetChild(i), scene);
    }
}

void FBXImporter::importNodeAttribute(const FbxNode* node,
                                      const FbxNodeAttribute* attribute, Scene &scene) {
    FbxNodeAttribute::EType type = attribute->GetAttributeType();
    
    if (type == FbxNodeAttribute::eMesh) {
        const FbxMesh* mesh = dynamic_cast<const FbxMesh*>(attribute);
        
        if (!mesh->IsTriangleMesh()) {
            return;
        }
        
//        const FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();
//        //if (normalElement->GetMappingMode() != FbxGeometryElement::eByControlPoint) {
//        qDebug() << node->GetName() << normalElement->GetMappingMode();
//        //}
//        return;
//        
//        // Get normals and uvs arrays
//        const FbxLayer* layer = mesh->GetLayer(0);
//        const FbxLayerElementNormal* normalsLayer = layer->GetNormals();
//        const FbxLayerElementUV* uvsLayer = layer->GetUVs();

        // Load vertices
        uint_t verticesCount = mesh->GetControlPointsCount();
        Vertex* vertices = new Vertex[verticesCount];
        for (uint_t i = 0; i < verticesCount; ++i) {
            vertices[i].position = importVec3(mesh->GetControlPointAt(i));
        }
        
        // Load indices
        uint_t indicesCount = mesh->GetPolygonVertexCount();
        uint_t* indices = new uint_t[indicesCount];
        int* polygonIndices = mesh->GetPolygonVertices();
        for (uint_t i = 0; i < indicesCount; ++i) {
            indices[i] = polygonIndices[i];
        }
        
        qDebug() << verticesCount << indicesCount;
        
        // Load normals
        const FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();
        if (normalElement) {
            if (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
                for (uint_t i = 0; i < verticesCount; ++i) {
                    int normalIndex = 0;
                    if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
                        normalIndex = i;
                    } else if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
                        normalIndex = normalElement->GetIndexArray().GetAt(i);
                    }
                    
                    vertices[i].normal = importVec3(normalElement->GetDirectArray().GetAt(normalIndex));
                }
            } else if (normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
                for (uint_t i = 0; i < indicesCount; ++i) {
                    uint_t vertexIndex = i;
                    int normalIndex = 0;
                    if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
                        normalIndex = vertexIndex;
                    } else if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
                        normalIndex = normalElement->GetIndexArray().GetAt(vertexIndex);
                    }
                    
                    vertices[vertexIndex].normal = importVec3(normalElement->GetDirectArray().GetAt(normalIndex));
                }
            }
        }
    }
}

vec3 FBXImporter::importVec3(const FbxVector4& v) {
    return vec3(v[0], v[1], v[2]);
}

vec2 FBXImporter::importVec2(const FbxVector2& v) {
    return vec2(v[0], v[1]);
}