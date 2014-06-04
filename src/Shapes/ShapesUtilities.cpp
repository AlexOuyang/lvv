//
//  ShapesUtilities.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#include "Core/Core.h"

#include "ShapesUtilities.h"

std::shared_ptr<Mesh> ShapesUtilities::CreateBox(float x, float y, float z) {
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
    
    // Allocate arrays
    int verticesCount = 24;
    int trianglesCount = 12;

	Vertex* vertices = new Vertex[verticesCount];
	uint_t* indices = new uint_t[trianglesCount*3];
        
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
	indices[0*3+0] = 0;
    indices[0*3+1] = 1;
    indices[0*3+2] = 2;
    indices[1*3+0] = 0;
    indices[1*3+1] = 2;
    indices[1*3+2] = 3;
    
	// Left
	vertices[4] = Vertex(p000, -vec3(1, 0, 0), t00);
	vertices[5] = Vertex(p001, -vec3(1, 0, 0), t10);
	vertices[6] = Vertex(p011, -vec3(1, 0, 0), t11);
	vertices[7] = Vertex(p010, -vec3(1, 0, 0), t01);
	indices[2*3+0] = 4;
    indices[2*3+1] = 5;
    indices[2*3+2] = 6;
    indices[3*3+0] = 4;
    indices[3*3+1] = 6;
    indices[3*3+2] = 7;
    
	// Top
	vertices[8] = Vertex(p011, vec3(0, 1, 0), t00);
	vertices[9] = Vertex(p111, vec3(0, 1, 0), t10);
	vertices[10] = Vertex(p110, vec3(0, 1, 0), t11);
	vertices[11] = Vertex(p010, vec3(0, 1, 0), t01);
	indices[4*3+0] = 8;
    indices[4*3+1] = 9;
    indices[4*3+2] = 10;
    indices[5*3+0] = 8;
    indices[5*3+1] = 10;
    indices[5*3+2] = 11;
    
	// Bottom
	vertices[12] = Vertex(p000, -vec3(0, 1, 0), t00);
	vertices[13] = Vertex(p100, -vec3(0, 1, 0), t10);
	vertices[14] = Vertex(p101, -vec3(0, 1, 0), t11);
	vertices[15] = Vertex(p001, -vec3(0, 1, 0), t01);
	indices[6*3+0] = 12;
    indices[6*3+1] = 13;
    indices[6*3+2] = 14;
    indices[7*3+0] = 12;
    indices[7*3+1] = 14;
    indices[7*3+2] = 15;
    
	// Front
	vertices[16] = Vertex(p001, vec3(0, 0, 1), t00);
	vertices[17] = Vertex(p101, vec3(0, 0, 1), t10);
	vertices[18] = Vertex(p111, vec3(0, 0, 1), t11);
	vertices[19] = Vertex(p011, vec3(0, 0, 1), t01);
	indices[8*3+0] = 16;
    indices[8*3+1] = 17;
    indices[8*3+2] = 18;
    indices[9*3+0] = 16;
    indices[9*3+1] = 18;
    indices[9*3+2] = 19;
    
	// Back
	vertices[20] = Vertex(p100, -vec3(0, 0, 1), t00);
	vertices[21] = Vertex(p000, -vec3(0, 0, 1), t10);
	vertices[22] = Vertex(p010, -vec3(0, 0, 1), t11);
	vertices[23] = Vertex(p110, -vec3(0, 0, 1), t01);
	indices[10*3+0] = 20;
    indices[10*3+1] = 21;
    indices[10*3+2] = 22;
    indices[11*3+0] = 20;
    indices[11*3+1] = 22;
    indices[11*3+2] = 23;
    
    mesh->setVertices(verticesCount, vertices);
    mesh->setIndices(trianglesCount*3, indices);
    
    // Generate mesh tangents
    mesh->generateTangents();
    
    return mesh;
}