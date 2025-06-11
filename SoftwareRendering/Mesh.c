#include "Mesh.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

mesh_t mesh = {
	.vertices = NULL,
	.faces = NULL,
	.rotation = { 0, 0, 0 },
	.scale = {1.0, 1.0, 1.0 },
	.translation = { 0, 0, 0 }
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
	{.x = -1, .y = -1, .z = -1 }, // 1
	{.x = -1, .y = 1, .z = -1 }, // 2
	{.x = 1, .y = 1, .z = -1 }, // 3
	{.x = 1, .y = -1, .z = -1 }, // 4
	{.x = 1, .y = 1, .z = 1 }, // 5
	{.x = 1, .y = -1, .z = 1 }, // 6
	{.x = -1, .y = 1, .z = 1 }, // 7
	{.x = -1, .y = -1, .z = 1 }  // 8
};

face_t cube_faces[N_CUBE_FACES] = {
	// front
	{.a = 1, .b = 2, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	{.a = 1, .b = 3, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },

	// right
	{.a = 4, .b = 3, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	{.a = 4, .b = 5, .c = 6, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },

	// back
	{.a = 6, .b = 5, .c = 7, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	{.a = 6, .b = 7, .c = 8, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },

	// left
	{.a = 8, .b = 7, .c = 2, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	{.a = 8, .b = 2, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },

	// top
	{.a = 2, .b = 7, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	{.a = 2, .b = 5, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },

	// bottom
	{.a = 6, .b = 8, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	{.a = 6, .b = 1, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF }
};

// TODO: Create implementation for mesh.h functions
void load_cube_mesh_data(void)
{
	//--Looping all the vertices
	for (int i = 0; i < N_CUBE_VERTICES; i++)
	{
		vec3_t cube_vertex = cube_vertices[i];
		array_push(mesh.vertices, cube_vertex);
	}

	//--Looping all the faces
	for (int i = 0; i < N_CUBE_FACES; i++)
	{
		face_t cube_face = cube_faces[i];
		array_push(mesh.faces, cube_face);
	}
}

void load_obj_file_data(char* filename)
{
	FILE* file = NULL;
	errno_t err = fopen_s(&file, filename, "r");

	if (err != 0 || file == NULL) {
		printf("Failed to open file: %s (errno: %d)\n", filename, err);
		return;
	}

	tex2_t* texcoords = NULL; // must be initialized
	char line[1024];

	while (fgets(line, sizeof(line), file)) {
		// Parse vertex position
		if (strncmp(line, "v ", 2) == 0) {
			vec3_t vertex;
			sscanf_s(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
			array_push(mesh.vertices, vertex); // ✅ // assign back
		}

		// Parse texture coordinate
		else if (strncmp(line, "vt ", 3) == 0) {
			tex2_t texcoord;
			sscanf_s(line, "vt %f %f", &texcoord.u, &texcoord.v);
			array_push(texcoords, texcoord); // must assign back
		}

		// Parse face
		else if (strncmp(line, "f ", 2) == 0) {
			int vertex_indices[3];
			int texture_indices[3];
			int normal_indices[3];

			int parsed = sscanf_s(
				line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&vertex_indices[0], &texture_indices[0], &normal_indices[0],
				&vertex_indices[1], &texture_indices[1], &normal_indices[1],
				&vertex_indices[2], &texture_indices[2], &normal_indices[2]
			);

			if (parsed == 9) {
				face_t face = {
					.a = vertex_indices[0] - 1,
					.b = vertex_indices[1] - 1,
					.c = vertex_indices[2] - 1,
					.a_uv = texcoords ? texcoords[texture_indices[0] - 1] : (tex2_t) { 0 },
					.b_uv = texcoords ? texcoords[texture_indices[1] - 1] : (tex2_t) { 0 },
					.c_uv = texcoords ? texcoords[texture_indices[2] - 1] : (tex2_t) { 0 },
					.color = 0xFFFFFFFF
				};
				array_push(mesh.faces, face); // assign back
			}
			else {
				printf("Warning: Invalid face format: %s", line);
			}
		}
	}

	array_free(texcoords);
	fclose(file);
}