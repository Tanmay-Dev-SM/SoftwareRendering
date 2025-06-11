#include "debug_controller.h"
#include <stdio.h>

// Global instance
debug_controller_t debug = {
	.paused = false,
	.single_step = false,
	.wait_for_key = false,
	.step_delay_ms = 0,
	.step_count = 0,
	.remaining_pixel_steps = 0,
	.frame_step_armed = false
};

void debug_toggle_pause(void) {
	debug.paused = !debug.paused;
	printf("[DEBUG] Toggled %s\n", debug.paused ? "Paused" : "Running");
}

void debug_step_frame(void) {
	if (debug.paused) {
		debug.frame_step_armed = true;
		printf("[DEBUG] Frame step requested\n");
	}
}

void debug_step_pixel(int steps) {
	if (debug.paused) {
		debug.remaining_pixel_steps += steps;
		printf("[DEBUG] Pixel steps += %d (remaining: %d)\n", steps, debug.remaining_pixel_steps);
	}
}

bool debug_should_render_frame(void) {
	return !debug.paused || debug.frame_step_armed || debug.remaining_pixel_steps > 0;
}

void debug_wait_for_step(void) {
	if (debug.wait_for_key) {
		printf("[DEBUG] Waiting for ENTER to continue...\n");
		SDL_Event event;
		while (SDL_WaitEvent(&event)) {
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
				break;
			}
		}
	}

	if (debug.step_delay_ms > 0) {
		SDL_Delay(debug.step_delay_ms);
	}

	if (debug.frame_step_armed) {
		debug.frame_step_armed = false;
	}

	debug.step_count++;
}

void debug_set_step_delay(int ms) {
	debug.step_delay_ms = ms;
	printf("[DEBUG] Step delay set to %d ms\n", ms);
}

void debug_enable_wait_for_key(bool enable) {
	debug.wait_for_key = enable;
	printf("[DEBUG] Wait-for-key %s\n", enable ? "enabled" : "disabled");
}