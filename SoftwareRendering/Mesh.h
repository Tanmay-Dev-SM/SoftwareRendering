#pragma once
#include "Vector.h"
#include "Triangle.h"
#include "array.h"

enum { N_CUBE_VERTICES = 8 };
enum { N_CUBE_FACES = 6 * 2 };

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];

typedef struct {
	vec3_t* vertices; //--dynamic array of vertices
	face_t* faces; //--dynamic array of faces
	vec3_t rotation; //--roation with x, y and z values in the mesh
	vec3_t scale; //--scale with x, y and z values
	vec3_t translation; //--transalation with x, y, and z values
} mesh_t;

extern mesh_t mesh; //--Global Mesh Object

void load_cube_mesh_data(void);

void load_obj_file_data(char* filename);
