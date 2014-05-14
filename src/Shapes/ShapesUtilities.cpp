//
//  ShapesUtilities.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#include "Core/Core.h"

#include "ShapesUtilities.h"

Mesh* ShapesUtilities::CreateBox(float x, float y, float z) {
    Mesh* mesh = new Mesh();
    
    // Allocate arrays
    int verticesCount = 24;
    int trianglesCount = 12;

	Vertex* vertices = new Vertex[verticesCount];
	Triangle* triangles = new Triangle[trianglesCount];
    
    for (int i = 0; i < trianglesCount; ++i) {
        triangles[i].setMesh(mesh);
    }
    
	x *= 0.5f;
	y *= 0.5f;
	z *= 0.5f;
    
	// Corners
	vec3 p000(-x,-y,-z);
	vec3 p001(-x,-y,z);
	vec3 p010(-x,y,-z);
	vec3 p011(-x,y,z);
	vec3 p100(x,-y,-z);
	vec3 p101(x,-y,z);
	vec3 p110(x,y,-z);
	vec3 p111(x,y,z);
    
	// Tex coords
	vec2 t00(0.0f,0.0f);
	vec2 t01(0.0f,1.0f);
	vec2 t10(1.0f,0.0f);
	vec2 t11(1.0f,1.0f);
    
	// Right
	vertices[0] = Vertex(p101, vec3(1, 0, 0), t00);
	vertices[1] = Vertex(p100, vec3(1, 0, 0), t10);
	vertices[2] = Vertex(p110, vec3(1, 0, 0), t11);
	vertices[3] = Vertex(p111, vec3(1, 0, 0), t01);
	triangles[0] = Triangle(&vertices[0], &vertices[1], &vertices[2]);
	triangles[1] = Triangle(&vertices[0], &vertices[2], &vertices[3]);
    
	// Left
	vertices[4] = Vertex(p000, -vec3(1, 0, 0), t00);
	vertices[5] = Vertex(p001, -vec3(1, 0, 0), t10);
	vertices[6] = Vertex(p011, -vec3(1, 0, 0), t11);
	vertices[7] = Vertex(p010, -vec3(1, 0, 0), t01);
	triangles[2] = Triangle(&vertices[4], &vertices[5], &vertices[6]);
	triangles[3] = Triangle(&vertices[4], &vertices[6], &vertices[7]);
    
	// Top
	vertices[8] = Vertex(p011, vec3(0, 1, 0), t00);
	vertices[9] = Vertex(p111, vec3(0, 1, 0), t10);
	vertices[10] = Vertex(p110, vec3(0, 1, 0), t11);
	vertices[11] = Vertex(p010, vec3(0, 1, 0), t01);
	triangles[4] = Triangle(&vertices[8], &vertices[9], &vertices[10]);
	triangles[5] = Triangle(&vertices[8], &vertices[10], &vertices[11]);
    
	// Bottom
	vertices[12] = Vertex(p000, -vec3(0, 1, 0), t00);
	vertices[13] = Vertex(p100, -vec3(0, 1, 0), t10);
	vertices[14] = Vertex(p101, -vec3(0, 1, 0), t11);
	vertices[15] = Vertex(p001, -vec3(0, 1, 0), t01);
	triangles[6] = Triangle(&vertices[12], &vertices[13], &vertices[14]);
	triangles[7] = Triangle(&vertices[12], &vertices[14], &vertices[15]);
    
	// Front
	vertices[16] = Vertex(p001, vec3(0, 0, 1), t00);
	vertices[17] = Vertex(p101, vec3(0, 0, 1), t10);
	vertices[18] = Vertex(p111, vec3(0, 0, 1), t11);
	vertices[19] = Vertex(p011, vec3(0, 0, 1), t01);
	triangles[8] = Triangle(&vertices[16], &vertices[17], &vertices[18]);
	triangles[9] = Triangle(&vertices[16], &vertices[18], &vertices[19]);
    
	// Back
	vertices[20] = Vertex(p100, -vec3(0, 0, 1), t00);
	vertices[21] = Vertex(p000, -vec3(0, 0, 1), t10);
	vertices[22] = Vertex(p010, -vec3(0, 0, 1), t11);
	vertices[23] = Vertex(p110, -vec3(0, 0, 1), t01);
	triangles[10] = Triangle(&vertices[20], &vertices[21], &vertices[22]);
	triangles[11] = Triangle(&vertices[20], &vertices[22], &vertices[23]);
    
    mesh->setVertices(verticesCount, vertices);
    mesh->setTriangles(trianglesCount, triangles);
    
    return mesh;
}