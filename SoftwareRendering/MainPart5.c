#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "upng.h"
#include "array.h"
#include "debug_controller.h"
#include "display.h"
#include "vector.h"
#include "Matrix.h"
#include "Triangle.h"
#include "mesh.h"
#include "Light.h"
#include "Texture.h"

///////////////////////////////////////////////////////////////////////////////
// Array of triangles that should be rendered frame by frame
///////////////////////////////////////////////////////////////////////////////
triangle_t* triangles_to_render = NULL;

///////////////////////////////////////////////////////////////////////////////
// Global variables for execution status and game loop
///////////////////////////////////////////////////////////////////////////////
bool is_running = false;
int previous_frame_time = 0;

vec3_t camera_position = { 0, 0, 0 };
mat4_t proj_matrix;

///////////////////////////////////////////////////////////////////////////////
// Setup function to initialize variables and game objects
///////////////////////////////////////////////////////////////////////////////
void setup(void) {
	// Initialize render mode and triangle cull methods
	//--Starting Variable
	render_method = RENDER_WIRE;
	cull_method = CULL_BACKFACE;

	// Allocate the required memory in bytes to hold the color buffer
	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);

	// Creating a SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	//--Initialize the perspective matrix
	float fov = M_PI / 3.0; //--!!!!Angle in Radians 180/3 or 60 deg
	float aspect = (float)window_height / (float)window_width;
	float snear = 0.1;
	float zfar = 100.0;
	proj_matrix = mat4_make_perspective(fov, aspect, snear, zfar);

	// Loads the vertex and face values for the mesh data structure
	//--For fixed Cube for testing
	//load_cube_mesh_data();
	load_obj_file_data("./assets/crab.obj");

	// Manually load the harcoded texture data from static array
	//mesh_texture = (uint32_t*)REDBRICK_TEXTURE;
	//texture_width = 64;
	//texture_height = 64;

	// Loading Texture information from external file
	load_png_texture_data("./assets/crab.png");
}

///////////////////////////////////////////////////////////////////////////////
// Poll system events and handle keyboard input
///////////////////////////////////////////////////////////////////////////////
void process_input(void) {
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_QUIT:
		is_running = false;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE)
			is_running = false;
		if (event.key.keysym.sym == SDLK_1)
			render_method = RENDER_WIRE_VERTEX;
		if (event.key.keysym.sym == SDLK_2)
			render_method = RENDER_WIRE;
		if (event.key.keysym.sym == SDLK_3)
			render_method = RENDER_FILL_TRIANGLE;
		if (event.key.keysym.sym == SDLK_4)
			render_method = RENDER_FILL_TRIANGLE_WIRE;
		if (event.key.keysym.sym == SDLK_5)
			render_method = RENDER_TEXTURED;
		if (event.key.keysym.sym == SDLK_6)
			render_method = RENDER_TEXTURED_WIRE;
		if (event.key.keysym.sym == SDLK_q)
			render_method = RENDER_CORRECT_TEXTURED;
		if (event.key.keysym.sym == SDLK_w)
			render_method = RENDER_CORRECT_TEXTURED_WIRE;
		if (event.key.keysym.sym == SDLK_c)
			cull_method = CULL_BACKFACE;
		if (event.key.keysym.sym == SDLK_d)
			cull_method = CULL_NONE;
		// Pause/resume debug
		if (event.key.keysym.sym == SDLK_p) debug_toggle_pause();

		// Step by frame
		if (event.key.keysym.sym == SDLK_RETURN) debug_step_frame();

		// Pixel-level stepping: 1, 5, 10, 50
		if (event.key.keysym.sym == SDLK_7) debug_step_pixel(1);
		if (event.key.keysym.sym == SDLK_8) debug_step_pixel(5);
		if (event.key.keysym.sym == SDLK_9) debug_step_pixel(10);
		if (event.key.keysym.sym == SDLK_0) debug_step_pixel(50);
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Update function frame by frame with a fixed time step
///////////////////////////////////////////////////////////////////////////////
void update(void) {
	// Wait some time until the reach the target frame time in milliseconds
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

	// Only delay execution if we are running too fast
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}

	previous_frame_time = SDL_GetTicks();

	// Initialize the array of triangles to render
	triangles_to_render = NULL;

	mesh.rotation.x += 0.00;
	mesh.rotation.y += 0.01;
	mesh.rotation.z += 0.00;

	//mesh.scale.x += 0.002;
	//mesh.scale.y += 0.001;

	//mesh.translation.x += 0.01;

	mesh.translation.z = 5;//--Used for camera

	//--Creating a scale, translation and rotation matrix that will be used to multiply the mesh vertices
	mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

	// Loop all triangle faces of our mesh
	int num_faces = array_length(mesh.faces);
	for (int i = 0; i < num_faces; i++) {
		face_t mesh_face = mesh.faces[i];

		vec3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a];
		face_vertices[1] = mesh.vertices[mesh_face.b];
		face_vertices[2] = mesh.vertices[mesh_face.c];

		vec4_t transformed_vertices[3];

		// Loop all three vertices of this current face and apply transformations
		for (int j = 0; j < 3; j++) {
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);
			//--Old Code
			/*
			// Translate the vertices away from the camera
			//transformed_vertex.z += 5;
			//--Will Be using Matrix instead of manulling using vertices
			//--Using Matrices for all Transformations (Orders Matter)
			//--Using Scale Matrix
			transformed_vertex = mat4_mul_vec4(scale_matrix, transformed_vertex);
			//--Using Rotation Matrix
			transformed_vertex = mat4_mul_vec4(rotation_matrix_x, transformed_vertex);
			transformed_vertex = mat4_mul_vec4(rotation_matrix_y, transformed_vertex);
			transformed_vertex = mat4_mul_vec4(rotation_matrix_z, transformed_vertex);
			//--Using Translation Matrix
			transformed_vertex = mat4_mul_vec4(translation_matrix, transformed_vertex);
			*/

			//--Using World Matrix Approach
			//--Create a World Matrix combining scale, rotation and translation
			mat4_t world_matrix = mat4_identity();

			//--Multiply all matices and load the world matrix
			//--Orders Matters: First Scale, then rotate, then translate
			world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
			world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

			//--Multiply the world matrix by the original vector
			transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

			// Save transformed vertex in the array of transformed vertices
			transformed_vertices[j] = transformed_vertex;
		}

		// Check backface culling
		vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
		vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
		vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);/* C---B */

		// Get the vector subtraction of B-A and C-A
		vec3_t vector_ab = vec3_sub(vector_b, vector_a);
		vec3_t vector_ac = vec3_sub(vector_c, vector_a);
		vec3_normalize(&vector_ab);
		vec3_normalize(&vector_ac);

		// Compute the face normal (using cross product to find perpendicular)
		vec3_t normal = vec3_cross(vector_ab, vector_ac); //--When Using Fixed Cube flip the vectors

		// Normalize the face normal vector
		vec3_normalize(&normal);

		// Find the vector between a point in the triangle and the camera origin
		vec3_t camera_ray = vec3_sub(camera_position, vector_a);

		// Calculate how aligned the camera ray is with the face normal (using dot product)
		float dot_normal_camera = vec3_dot(normal, camera_ray);

		if (cull_method == CULL_BACKFACE) {
			// Bypass the triangles that are looking away from the camera
			// the equality signs tells which orintations to use
			if (dot_normal_camera < 0) {
				continue;
			}
		}

		vec4_t projected_points[3];

		// Loop all three vertices to perform projection
		for (int j = 0; j < 3; j++) {
			projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);

			//--Orders Matter
			//--Scale into the view
			projected_points[j].x *= (window_width / 2.0);
			projected_points[j].y *= (window_height / 2.0);

			//--Invert the y values in screen space for flipeed screen y coordinate
			projected_points[j].y *= -1;

			//--Translate the points to middle of screen
			projected_points[j].x += (window_width / 2.0);
			projected_points[j].y += (window_height / 2.0);
		}

		//--Caluclate the average depth for each face based on the vertices after transformation (hacky!! aproach)
		float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

		//--Calculate the shade intensity based on how aligned is the face normal and inverse of light direction
		float light_intensity_factor = -vec3_dot(normal, light.direction);

		//--Calculate the triangle color based on the light angle
		uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

		triangle_t projected_triangle = {
			.points = {
				{projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w},
				{projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w},
				{projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w}
				},
			.texcoords = {
				{mesh_face.a_uv.u, mesh_face.a_uv.v},
				{mesh_face.b_uv.u, mesh_face.b_uv.v},
				{mesh_face.c_uv.u, mesh_face.c_uv.v},
			},
			.color = triangle_color,
			.avg_depth = avg_depth
		};

		// Save the projected triangle in the array of triangles to render
		array_push(triangles_to_render, projected_triangle);//--change the projected_triangle to projected_triangle_fixed
	}
	//--Sort the triangle to render by their avg_depth in acsending order
	//--(far z render first close z render last - Painter Algorithm)
	//--Right now using bubble sort
	int num_triangles = array_length(triangles_to_render);
	if (triangles_to_render != NULL && num_triangles > 1) {
		for (int i = 0; i < num_triangles; i++) {
			for (int j = i; j < num_triangles; j++) {
				if (triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth) {
					triangle_t temp = triangles_to_render[i];
					triangles_to_render[i] = triangles_to_render[j];
					triangles_to_render[j] = temp;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Render function to draw objects on the display
///////////////////////////////////////////////////////////////////////////////
void render(void) {
	/// Clear buffer ONLY if not paused or pixel stepping
	if (!debug.paused || debug.remaining_pixel_steps == 0) {
		clear_color_buffer(0xFF000000);
	}

	debug.step_count = 0;
	//draw_grid();

	// Loop all projected triangles and render them
	int num_triangles = array_length(triangles_to_render);
	for (int i = 0; i < num_triangles; i++) {
		triangle_t triangle = triangles_to_render[i];

		// Draw filled triangle
		//-- Right now we are not sending z and w values because we don't need it for now but to render correct perspective textures we will need those
		/*0xFF555555*/
		if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE) {
			draw_filled_triangle(
				triangle.points[0].x, triangle.points[0].y, // vertex A
				triangle.points[1].x, triangle.points[1].y, // vertex B
				triangle.points[2].x, triangle.points[2].y, // vertex C
				//0xFF555555//--Use triangle.color
				triangle.color
			);
		}

		// Draw Textured Triangle
		//-- Right now we are not sending z and w values because we don't need it for now but to render correct perspective textures we will need those
		if (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE) {
			draw_texture_triangle(
				triangle.points[0].x, triangle.points[0].y, triangle.texcoords[0].u, triangle.texcoords[0].v, // vertex A
				triangle.points[1].x, triangle.points[1].y, triangle.texcoords[1].u, triangle.texcoords[1].v, // vertex B
				triangle.points[2].x, triangle.points[2].y, triangle.texcoords[2].u, triangle.texcoords[2].v, // vertex C
				mesh_texture
			);
		}

		// Draw Textures Triangle with correct texture persepective
		if (render_method == RENDER_CORRECT_TEXTURED || render_method == RENDER_CORRECT_TEXTURED_WIRE) {
			draw_texture_triangle_correct_persepective(
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v, // vertex A
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v, // vertex B
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v, // vertex C
				mesh_texture
			);
		}

		// Draw triangle wireframe
		//-- Right now we are not sending z and w values because we don't need it for now but to render correct perspective textures we will need those
		if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE || render_method == RENDER_CORRECT_TEXTURED_WIRE) {
			draw_triangle(
				triangle.points[0].x, triangle.points[0].y, // vertex A
				triangle.points[1].x, triangle.points[1].y, // vertex B
				triangle.points[2].x, triangle.points[2].y, // vertex C
				0xFFFFFFFF
			);
		}

		// Draw triangle vertex points
		if (render_method == RENDER_WIRE_VERTEX) {
			draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000); // vertex A
			draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000); // vertex B
			draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000); // vertex C
		}
	}

	//draw_triangle(300, 100, 50, 400, 500, 700, 0xFF00FF00);

	//draw_filled_triangle(300, 100, 50, 400, 500, 700, 0xFF00FF00);

	// Clear the array of triangles to render every frame loop
	array_free(triangles_to_render);

	// Only render color buffer if something was drawn
	if (debug_should_render_frame()) {
		render_color_buffer();
		SDL_RenderPresent(renderer);
		clear_color_buffer(0xFF000000);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Free the memory that was dynamically allocated by the program
///////////////////////////////////////////////////////////////////////////////
void free_resources(void) {
	free(color_buffer);
	upng_free(png_texture);
	array_free(mesh.faces);
	array_free(mesh.vertices);
}

///////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////
int main(void) {
	is_running = initialize_window();
	setup();

	while (is_running) {
		process_input();

		if (debug_should_render_frame()) {
			update();
			render();
			debug_wait_for_step();
		}
	}

	destroy_Window();
	free_resources();
	return 0;
}