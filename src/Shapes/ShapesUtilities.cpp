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
    mesh->verticesCount = 24;
    mesh->trianglesCount = 12;

	mesh->vertices = new Vertex[mesh->verticesCount];
	mesh->triangles = new Triangle[mesh->trianglesCount];
    
    for (int i = 0; i < mesh->trianglesCount; ++i) {
        mesh->triangles[i].mesh = mesh;
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
	mesh->vertices[0] = Vertex(p101, vec3(1, 0, 0), t00);
	mesh->vertices[1] = Vertex(p100, vec3(1, 0, 0), t10);
	mesh->vertices[2] = Vertex(p110, vec3(1, 0, 0), t11);
	mesh->vertices[3] = Vertex(p111, vec3(1, 0, 0), t01);
	mesh->triangles[0] = Triangle(&mesh->vertices[0], &mesh->vertices[1], &mesh->vertices[2]);
	mesh->triangles[1] = Triangle(&mesh->vertices[0], &mesh->vertices[2], &mesh->vertices[3]);
    
	// Left
	mesh->vertices[4] = Vertex(p000, -vec3(1, 0, 0), t00);
	mesh->vertices[5] = Vertex(p001, -vec3(1, 0, 0), t10);
	mesh->vertices[6] = Vertex(p011, -vec3(1, 0, 0), t11);
	mesh->vertices[7] = Vertex(p010, -vec3(1, 0, 0), t01);
	mesh->triangles[2] = Triangle(&mesh->vertices[4], &mesh->vertices[5], &mesh->vertices[6]);
	mesh->triangles[3] = Triangle(&mesh->vertices[4], &mesh->vertices[6], &mesh->vertices[7]);
    
	// Top
	mesh->vertices[8] = Vertex(p011, vec3(0, 1, 0), t00);
	mesh->vertices[9] = Vertex(p111, vec3(0, 1, 0), t10);
	mesh->vertices[10] = Vertex(p110, vec3(0, 1, 0), t11);
	mesh->vertices[11] = Vertex(p010, vec3(0, 1, 0), t01);
	mesh->triangles[4] = Triangle(&mesh->vertices[8], &mesh->vertices[9], &mesh->vertices[10]);
	mesh->triangles[5] = Triangle(&mesh->vertices[8], &mesh->vertices[10], &mesh->vertices[11]);
    
	// Bottom
	mesh->vertices[12] = Vertex(p000, -vec3(0, 1, 0), t00);
	mesh->vertices[13] = Vertex(p100, -vec3(0, 1, 0), t10);
	mesh->vertices[14] = Vertex(p101, -vec3(0, 1, 0), t11);
	mesh->vertices[15] = Vertex(p001, -vec3(0, 1, 0), t01);
	mesh->triangles[6] = Triangle(&mesh->vertices[12], &mesh->vertices[13], &mesh->vertices[14]);
	mesh->triangles[7] = Triangle(&mesh->vertices[12], &mesh->vertices[14], &mesh->vertices[15]);
    
	// Front
	mesh->vertices[16] = Vertex(p001, vec3(0, 0, 1), t00);
	mesh->vertices[17] = Vertex(p101, vec3(0, 0, 1), t10);
	mesh->vertices[18] = Vertex(p111, vec3(0, 0, 1), t11);
	mesh->vertices[19] = Vertex(p011, vec3(0, 0, 1), t01);
	mesh->triangles[8] = Triangle(&mesh->vertices[16], &mesh->vertices[17], &mesh->vertices[18]);
	mesh->triangles[9] = Triangle(&mesh->vertices[16], &mesh->vertices[18], &mesh->vertices[19]);
    
	// Back
	mesh->vertices[20] = Vertex(p100, -vec3(0, 0, 1), t00);
	mesh->vertices[21] = Vertex(p000, -vec3(0, 0, 1), t10);
	mesh->vertices[22] = Vertex(p010, -vec3(0, 0, 1), t11);
	mesh->vertices[23] = Vertex(p110, -vec3(0, 0, 1), t01);
	mesh->triangles[10] = Triangle(&mesh->vertices[20], &mesh->vertices[21], &mesh->vertices[22]);
	mesh->triangles[11] = Triangle(&mesh->vertices[20], &mesh->vertices[22], &mesh->vertices[23]);
    
    return mesh;
}