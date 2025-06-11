#pragma once
#include <stdbool.h>
#include <SDL.h>

/// Debug controller struct to manage stepping
typedef struct {
	bool paused;                  // Whether rendering is paused
	bool single_step;            // Step by frame (one full render)
	bool wait_for_key;           // Wait for key press (ENTER) before continuing
	int step_delay_ms;           // Optional delay between steps
	int step_count;              // Total steps taken
	int remaining_pixel_steps;   // Step-by-pixel count
	bool frame_step_armed;       // Used to trigger frame step
} debug_controller_t;

// Global instance
extern debug_controller_t debug;

// Controls
void debug_toggle_pause(void);
void debug_step_frame(void);
void debug_step_pixel(int steps);
bool debug_should_render_frame(void);
void debug_wait_for_step(void);
void debug_set_step_delay(int ms);
void debug_enable_wait_for_key(bool enable);