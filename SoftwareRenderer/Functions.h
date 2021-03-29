#pragma once
#include"Includes.h"

vector_t vector_sub(const vector_t& x, const vector_t& y);
vector_t operator - (const vector_t& x, const vector_t& y);
vector_t vector_normalize(const vector_t& v);
float vector_length(const vector_t& v);
vector_t vector_cross(const vector_t& x, const vector_t& y);
float vector_dotProduct(const vector_t& x, const vector_t& y);
float operator*(const vector_t& x, const vector_t& y);

void matrix_set_identity(matrix_t* m);
void matrix_set_perspective(matrix_t* m, float fovy, float aspect, float z_near, float z_far);
void matrix_set_zero(matrix_t* m);
matrix_t matrix_mul(const matrix_t& left, const matrix_t& right);
matrix_t operator*(const matrix_t& left, const matrix_t& right);
vector_t matrix_apply(const vector_t& x, const matrix_t& m);
vector_t operator*(const vector_t& x, const matrix_t& m);
matrix_t matrix_translate_build(float x, float y, float z);
matrix_t matrix_rotate_build(float angle, const vector_t& v);
matrix_t matrix_transpose(const matrix_t& m);

vector_t transform_apply(const vector_t& x, const transform_t& ts);
vector_t operator*(const vector_t& x, const transform_t& ts);
void transform_init(transform_t* ts, int width, int height);


vector_t viewport_transform(const vector_t& x, const transform_t& ts);
int check_cvv(const vector_t& v);

inline int CMID(int x, int min, int max) { return (x < min) ? min : ((x > max) ? max : x); }

float interp(float length_total, float length_place, float x1, float x2);

inline UINT32 color_trans_255(const color_t &color) {
	int R = CMID((int)(color.r * 255.0f), 0, 255);
	int G = CMID((int)(color.g * 255.0f), 0, 255);
	int B = CMID((int)(color.b * 255.0f), 0, 255);
	int A = CMID((int)(color.a * 255.0f), 0, 255);
	return (UINT32)((A << 24) | (R << 16) | (G << 8) | (B));
}