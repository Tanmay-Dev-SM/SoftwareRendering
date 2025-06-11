#pragma once

typedef struct {
	float x;
	float y;
} vec2_t;

typedef struct {
	float x;
	float y;
	float z;
} vec3_t;

typedef struct {
	float x;
	float y;
	float z;
	float w;
}vec4_t;

//TODO: Add functions to manipullate vectors 2D and 3D

//--Vector 2D Functions
float vec2_length(vec2_t v);//--Calculate the lenght of vector (magnitude)

vec2_t vec2_add(vec2_t a, vec2_t b);//--Addition Function
vec2_t vec2_sub(vec2_t a, vec2_t b);//--Subtraction Function

vec2_t vec2_mul(vec2_t v, float factor);//--Multiplication Function
vec2_t vec2_div(vec2_t v, float factor);//--Division Function

float vec2_dot(vec2_t a, vec2_t b);//--Dot Product
void vec2_normalize(vec2_t* v);//--Normalize Opertion

//--Vector 3D Functions
float vec3_length(vec3_t v);//--Calculate the lenght of vector (magnitude)

vec3_t vec3_add(vec3_t a, vec3_t b);//--Addition Function
vec3_t vec3_sub(vec3_t a, vec3_t b);//--Subtraction Function

vec3_t vec3_mul(vec3_t v, float factor);//--Multiplication Function
vec3_t vec3_div(vec3_t v, float factor);//--Division Function

//?? cross product exists only for vec3_t
vec3_t vec3_cross(vec3_t a, vec3_t b);//--Cross Product
float vec3_dot(vec3_t a, vec3_t b);//--Dot Product
void vec3_normalize(vec3_t* v);//--Normalize Operation

//--Vector conversion Functions
vec4_t vec4_from_vec3(vec3_t v);
vec3_t vec3_from_vec4(vec4_t v);
vec2_t vec2_from_vec4(vec4_t v);

//--Rotation Functions
vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);