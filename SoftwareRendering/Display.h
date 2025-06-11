#pragma once
#include <stdio.h>
#include <stdint.h>//This gives uint8_t datatype
#include <stdbool.h>// C does not have bool dataType By default
#include <SDL.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

//--Global Variable to hold the Render States
enum cull_method {
	CULL_NONE,
	CULL_BACKFACE
} cull_method;

enum render_method {
	RENDER_WIRE,
	RENDER_WIRE_VERTEX,
	RENDER_FILL_TRIANGLE,
	RENDER_FILL_TRIANGLE_WIRE,
	RENDER_TEXTURED,
	RENDER_TEXTURED_WIRE,
	RENDER_CORRECT_TEXTURED,
	RENDER_CORRECT_TEXTURED_WIRE
}render_method;

//extern keyword means that the actual value will be defined in the implementation file generarlly in .c file
extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern uint32_t* color_buffer;//Creating an Raw Pointer Based Array
extern SDL_Texture* color_buffer_texture;

extern uint16_t window_width; //-- this is x coordinate the width goes from top to down
extern uint16_t window_height;//-- this is y coordinate the height goes from left to down

bool initialize_window(void);

void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void destroy_Window(void);

//User Custom Drawing Functions
void draw_grid(void);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_cross(int x, int y, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
