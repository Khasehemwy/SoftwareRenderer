#include"Functions.h"

void matrix_set_zero(matrix_t* m) {
	m->m[0][0] = m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
	m->m[1][0] = m->m[1][1] = m->m[1][2] = m->m[1][3] = 0.0f;
	m->m[2][0] = m->m[2][1] = m->m[2][2] = m->m[2][3] = 0.0f;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = m->m[3][3] = 0.0f;
}

matrix_t matrix_mul(const matrix_t* left, const matrix_t* right)
{
	matrix_t z;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			z.m[i][j] = (left->m[i][0] * right->m[0][j]) +
				(left->m[i][1] * right->m[1][j]) +
				(left->m[i][2] * right->m[2][j]) +
				(left->m[i][3] * right->m[3][j]);
		}
	}
	return z;
}
matrix_t operator*(const matrix_t& left, const matrix_t& right)
{
	return matrix_mul(&left, &right);
}

vector_t matrix_apply(const vector_t* x, const matrix_t* m)
{
	vector_t y;
	float X = x->x, Y = x->y, Z = x->z, W = x->w;
	y.x = X * m->m[0][0] + Y * m->m[1][0] + Z * m->m[2][0] + W * m->m[3][0];
	y.y = X * m->m[0][1] + Y * m->m[1][1] + Z * m->m[2][1] + W * m->m[3][1];
	y.z = X * m->m[0][2] + Y * m->m[1][2] + Z * m->m[2][2] + W * m->m[3][2];
	y.w = X * m->m[0][3] + Y * m->m[1][3] + Z * m->m[2][3] + W * m->m[3][3];
	return y;
}

vector_t operator*(const vector_t& x, const matrix_t& m)
{
	return matrix_apply(&x,&m);
}

vector_t transform_apply(const vector_t* x, const transform_t* ts)
{
	return (*x) * (ts->transform);
}

vector_t operator*(const vector_t& x, const transform_t& ts)
{
	return transform_apply(&x,&ts);
}

//return x-y
vector_t vector_sub(const vector_t* x, const vector_t* y)
{
	vector_t z;
	z.x = x->x - y->x;
	z.y = x->y - y->y;
	z.z = x->z - y->z;
	z.w = 1.0;
	return z;
}

vector_t operator-(const vector_t& x, const vector_t& y)
{
	return vector_sub(&x,&y);
}

vector_t vector_normalize(const vector_t* v)
{
	vector_t z=*v;
	float length = vector_length(v);
	if (length != 0.0f) {
		float inv = 1.0f / length;
		z.x *= inv;
		z.y *= inv;
		z.z *= inv;
	}
	return z;
}

float vector_length(const vector_t* v)
{
	float sq = v->x * v->x + v->y * v->y + v->z * v->z;
	return (float)sqrt(sq);
}

vector_t vector_cross(const vector_t* x, const vector_t* y)
{
	vector_t z;
	float m1, m2, m3;
	m1 = x->y * y->z - x->z * y->y;
	m2 = x->z * y->x - x->x * y->z;
	m3 = x->x * y->y - x->y * y->x;
	z.x = m1;
	z.y = m2;
	z.z = m3;
	z.w = 1.0f;
	return z;
}

void matrix_set_identity(matrix_t* m) {
	m->m[0][0] = m->m[1][1] = m->m[2][2] = m->m[3][3] = 1.0f;
	m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
	m->m[1][0] = m->m[1][2] = m->m[1][3] = 0.0f;
	m->m[2][0] = m->m[2][1] = m->m[2][3] = 0.0f;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0f;
}

void matrix_set_perspective(matrix_t* m, float fovy, float aspect, float z_near, float z_far){
	float fax = 1.0f / (float)tan(fovy * 0.5f);
	matrix_set_zero(m);
	m->m[0][0] = (float)(fax / aspect);
	m->m[1][1] = (float)(fax);
	m->m[2][2] = z_far / (z_far - z_near);
	m->m[3][2] = -z_near * z_far / (z_far - z_near);
	m->m[2][3] = 1;
}

void transform_init(transform_t* ts, int width, int height) {
	float aspect = (float)width / ((float)height);
	matrix_set_identity(&ts->world);
	matrix_set_identity(&ts->view);
	matrix_set_perspective(&ts->projection, 3.1415926f * 0.5f, aspect, 1.0f, 500.0f);
	ts->w = (float)width;
	ts->h = (float)height;
	ts->transform = ts->projection * ts->view * ts->world;//update
}

vector_t viewport_transform(const vector_t* x, const transform_t* ts)
{
	vector_t y;
	float rhw = 1.0f / x->w;
	y.x = (x->x * rhw + 1.0f) * ts->w * 0.5f;
	y.y = (1.0f - x->y * rhw) * ts->h * 0.5f;
	y.z = x->z * rhw;
	y.w = 1.0f;
	return y;
}

int check_cvv(const vector_t* v)
{
	float w = v->w;
	int check = 0;
	if (v->z < 0.0f) check |= 1;
	if (v->z > w) check |= 2;
	if (v->x < -w) check |= 4;
	if (v->x > w) check |= 8;
	if (v->y < -w) check |= 16;
	if (v->y > w) check |= 32;
	return check;
}