#include <stdio.h>
#include <stdint.h>//This gives uint8_t datatype
#include <stdbool.h>// C does not have bool dataType By default
#include <SDL.h>
#include "Display.h"
#include "Vector.h"
#include "Mesh.h"
#include "array.h"

/*
enum { N_POINTS = 9 * 9 * 9 };//Check ChatGPT for same solution to const problems
vec3_t cube_points[N_POINTS]; // 9x9x9 cube
vec2_t projected_points[N_POINTS];
*/

//--Triangles and Mesh Rendering
//triangle_t triangles_to_render[N_MESH_FACES];

//--Using Dynamic Array
triangle_t* triangle_to_render = NULL;

float fov_factor = 640;

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };
//vec3_t cube_rotaton = { .x = 0, .y = 0, .z = 0 };
//-- Now we are storing the rotation values in the mesh itself

//Decalring Global Variable So Can I track the instance As We need Only One Instance
bool is_running = false;

int previous_frame_time = 0;
int time_to_wait = 0;

void setup(void) {
	//Allocate the required memory in bytes to hold the color buffer
	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height); //--Color_buffer, Pixel_buffer and Frame_buffer Both Are same thing

	//?-Always Check If Malloc Worked
	if (!color_buffer) {
		fprintf(stderr, "Error creating SDL Renderer \n");
		return false;
	}

	//Creating a SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	//--Load the Mesh Data right now it is cube
	//load_cube_mesh_data();

	//--Loading the OBJ file for loading any 3D mesh
	load_obj_file_data("./assets/f22.obj");

	/*
	int point_count = 0;

	//Start Loading my array of vectors
	//From -1 to 1 (in this 9x9x9 cube)
	for (float x = -1; x <= 1; x += 0.25) {
		for (float y = -1; y <= 1; y += 0.25) {
			for (float z = -1; z <= 1; z += 0.25) {
				vec3_t new_point = { .x = x, .y = y, .z = z };
				cube_points[point_count] = new_point;
				point_count++;
			}
		}
	}
	*/

	vec3_t a = { 2.5, 6.4, 3.0 };
	vec3_t b = { -2.2, 1.4, -1.0 };

	float a_length = vec3_length(a);
	float b_length = vec3_length(b);
	vec3_t add_vector = vec3_add(a, b);

	printf("%f %f", a_length, b_length);
	printf("\n");
	printf("%f %f %f", add_vector.x, add_vector.y, add_vector.z);
}

void process_input(void) {
	SDL_Event event;
	SDL_PollEvent(&event);

	//Checking for Escape key and Terminate Button
	switch (event.type)
	{
	case SDL_QUIT:
		is_running = false;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			is_running = false;
		}
		break;
	}
}

vec2_t project(vec3_t point) {
	vec2_t projected_point = {
		.x = (fov_factor * point.x) / point.z, // here point.z gives us the prespective matrix, use the triangle similarity concept for it
		.y = (fov_factor * point.y) / point.z
	};
	return projected_point;
}

void update(void) {
	//--Manual Lock FrameTime
	//--while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));

	//--Implementing Delta Time for independent frame time logic
	//--previous_frame_time = SDL_GetTicks();

	//Better Version
	// Get the current time
	int current_time = SDL_GetTicks();

	// Calculate time elapsed since last frame
	int time_elapsed = current_time - previous_frame_time;

	// Compute the time to wait to match the frame target time
	int time_to_wait = FRAME_TARGET_TIME - time_elapsed;

	// Only delay if necessary
	if (time_to_wait > 0) {
		SDL_Delay(time_to_wait);
	}

	// Update previous frame time **after the delay**
	previous_frame_time = SDL_GetTicks();

	// Intialize the array of triangle to render
	triangle_to_render = NULL;

	//Updating the y axis for getting an animated y rotation frame by frame
	mesh.rotation.x += 0.01;
	mesh.rotation.y += 0.00;
	mesh.rotation.z += 0.00;

	//--Use actual Mesh Now
	//--Loop First Faces than Vertices
	int num_faces = array_length(mesh.faces);
	for (int i = 0; i < num_faces; i++)
	{
		face_t mesh_face = mesh.faces[i];

		vec3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a - 1];
		face_vertices[1] = mesh.vertices[mesh_face.b - 1];
		face_vertices[2] = mesh.vertices[mesh_face.c - 1];

		triangle_t projected_triangle;

		//-- Loop all three vertices of this current face and apply transformations
		for (int j = 0; j < 3; j++)
		{
			vec3_t transformed_vertex = face_vertices[j];

			transformed_vertex = vec3_roatate_x(transformed_vertex, mesh.rotation.x);
			transformed_vertex = vec3_roatate_y(transformed_vertex, mesh.rotation.y);
			transformed_vertex = vec3_roatate_z(transformed_vertex, mesh.rotation.z);

			//-- Translate the vertex away from the camera
			if (transformed_vertex.z < 0.1f) {
				transformed_vertex.z = 0.1f; // prevent projection explosion
			}

			transformed_vertex.z -= camera_position.z;

			//--Projection
			vec2_t projected_point = project(transformed_vertex);

			//-- Scale and Translate the projected Point to the middle of screen
			projected_point.x += (window_width / 2);
			projected_point.y += (window_height / 2);

			//--Save the Projected changes in the array
			projected_triangle.points[j] = projected_point;
		}

		//--Save the Projected changes in the array
		//-- triangles_to_render[i] = projected_triangle; Earlier Using the static array version
		array_push(triangle_to_render, projected_triangle); //--using dynamic array version
	}

	/*
	for (int i = 0; i < N_POINTS; i++) {
		vec3_t point = cube_points[i];

		vec3_t transformed_point = vec3_roatate_x(point, cube_rotaton.x);
		transformed_point = vec3_roatate_y(transformed_point, cube_rotaton.y);
		transformed_point = vec3_roatate_z(transformed_point, cube_rotaton.z);

		//Trnslate the points away from the camera
		transformed_point.z -= camera_position.z;

		//Project the Current Point
		vec2_t projected_point = project(transformed_point);

		//Save the Projected 2D vector in the array of projected points
		projected_points[i] = projected_point;
	}
	*/
}

void render(void) {
	//--Here Lies Custom Drawing Function Set By Users
	//draw_grid();
	//draw_pixel(20, 20, 0xFF00FFFF);//ARGB
	//draw_rect(40, 60, 100, 100, 0xFF00FFFF);

	/*
	for (int i = 0; i < N_POINTS; i++) {
		vec2_t projected_point = projected_points[i];
		draw_rect(
			projected_point.x + (window_width / 2),
			projected_point.y + (window_height / 2),
			4,
			4,
			0xFFFFFF00
		);
	}
	*/

	//--Loop all Projected Triangle and render the vertices
	int num_triangles = array_length(triangle_to_render);
	for (int i = 0; i < num_triangles; i++) {
		triangle_t triangle = triangle_to_render[i];
		draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
		draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
		draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

		int x0 = (int)floor(triangle.points[0].x);
		int y0 = (int)floor(triangle.points[0].y);
		int x1 = (int)floor(triangle.points[1].x);
		int y1 = (int)floor(triangle.points[1].y);
		int x2 = (int)floor(triangle.points[2].x);
		int y2 = (int)floor(triangle.points[2].y);
		draw_triangle(x0, y0, x1, y1, x2, y2, 0xFF00FF00);
	}

	//--Draw Line test
	/*
	draw_line(100, 200, 300, 50, 0xFF00FF00);
	draw_line(300, 50, 400, 600, 0xFF00FF00);
	draw_line(800, 800, 100, 100, 0xFF00FF00);
	*/

	//--Clear the Array of triangle to render after every frame loop
	array_free(triangle_to_render);

	render_color_buffer();
	clear_color_buffer(0xFF000000);//Clear the Color Buffer to Black

	//Present the Renderer
	SDL_RenderPresent(renderer);//Present the BackBuffer
}

//--Here we are cleaning everyting that we have created by ourself
void free_resources(void)
{
	free(color_buffer);
	array_free(mesh.faces);
	array_free(mesh.vertices);
}

int main(int argc, char* args[]) {
	is_running = initialize_window();

	setup();

	/*
	while (is_running) {
		process_input();
		update();
		render();
	}
	*/

	destroy_Window();
	free_resources();

	return 0;
}