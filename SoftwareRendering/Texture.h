#pragma once
#include <stdint.h>
#include "upng.h"

typedef struct {
	float u;
	float v;
}tex2_t;

extern int texture_width;
extern int texture_height;

//--Hardcode Texture Reading for testing
extern const uint8_t REDBRICK_TEXTURE[];
extern uint32_t* mesh_texture;

//--Using Upng Library for PNG image loading
extern upng_t* png_texture;

void load_png_texture_data(char* filename);