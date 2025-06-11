#include "Display.h"
#include "debug_controller.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

uint16_t window_width = 1024;
uint16_t window_height = 720;

bool initialize_window(void) {
	//Step 1: Initialize SDL (that is every driver specific things, Audio, Input etc)
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL. \n");
		return false;
	}

	//--USE SDL to query what is the fillscreen max Width and Height
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);

	//window_width = display_mode.w;
	//window_height = display_mode.h;

	//Step 2:Create A SDL Window
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		SDL_WINDOW_BORDERLESS
	);
	if (!window) {
		fprintf(stderr, "Error creating SDL Window \n");
		return false;
	}
	//Step 3:Create a SDL Renderer
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		fprintf(stderr, "Error creating SDL Renderer \n");
		return false;
	}
	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

	return true;
}

void render_color_buffer(void) {
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}
void clear_color_buffer(uint32_t color) {
	for (int y = 0; y < window_height; y++) {
		for (int x = 0; x < window_width; x++) {
			color_buffer[(window_width * y) + x] = color;
		}
	}
}
void destroy_Window(void) {
	//Also Remove Everything That We have Initialized But in Reverse Order
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void draw_grid(void) {
	for (int y = 0; y < window_height; y++) {
		for (int x = 0; x < window_width; x++) {
			if (x % 10 == 0 || y % 10 == 0) {
				color_buffer[(window_width * y) + x] = 0xFF333333;
			}
		}
	}
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			int current_x = x + i;
			int current_y = y + j;
			//color_buffer[(window_width * current_y) + current_x] = color;
			//--Use DrawPixel for manulpating the color_buffer
			draw_pixel(current_x, current_y, color);
		}
	}
}

void draw_pixel(int x, int y, uint32_t color) {
	if (debug.paused && debug.remaining_pixel_steps <= 0)
		return;

	if (debug.paused && debug.remaining_pixel_steps > 0)
		debug.remaining_pixel_steps--;

	if (x >= 0 && x < window_width && y >= 0 && y < window_height) {
		color_buffer[(window_width * y) + x] = color;

		// Force render immediately (ONLY if paused and stepping)
		if (debug.paused) {
			SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, window_width * sizeof(uint32_t));
			SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			SDL_Delay(debug.step_delay_ms); // Optional smoothness
		}
	}
}

void draw_cross(int x, int y, uint32_t color) {
	draw_pixel(x, y, color);             // center
	draw_pixel(x - 1, y, color);         // left
	draw_pixel(x + 1, y, color);         // right
	draw_pixel(x, y - 1, color);         // top
	draw_pixel(x, y + 1, color);         // bottom
}

//--Using DDA algorithm
void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);

	int longest_side_length = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

	float x_inc = delta_x / (float)longest_side_length;
	float y_inc = delta_y / (float)longest_side_length;

	float current_x = x0;
	float current_y = y0;

	for (int i = 0; i <= longest_side_length; i++)
	{
		draw_pixel((int)floor(current_x), (int)floor(current_y), color);
		current_x += x_inc;
		current_y += y_inc;
	}
}