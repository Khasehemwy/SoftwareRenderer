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