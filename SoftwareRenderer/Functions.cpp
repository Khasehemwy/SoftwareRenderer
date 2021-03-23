#include"Functions.h"

void matrix_set_zero(matrix_t* m) {
	m->m[0][0] = m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
	m->m[1][0] = m->m[1][1] = m->m[1][2] = m->m[1][3] = 0.0f;
	m->m[2][0] = m->m[2][1] = m->m[2][2] = m->m[2][3] = 0.0f;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = m->m[3][3] = 0.0f;
}

matrix_t matrix_mul(const matrix_t& left, const matrix_t& right)
{
	matrix_t z;
	matrix_t m_tmpl = right;
	matrix_t m_tmpr = left;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			z.m[j][i] = (m_tmpl.m[j][0] * m_tmpr.m[0][i]) +
				(m_tmpl.m[j][1] * m_tmpr.m[1][i]) +
				(m_tmpl.m[j][2] * m_tmpr.m[2][i]) +
				(m_tmpl.m[j][3] * m_tmpr.m[3][i]);
		}
	}
	return z;
}
matrix_t operator*(const matrix_t& left, const matrix_t& right)
{
	return matrix_mul(left, right);
}

vector_t matrix_apply(const matrix_t& m, const vector_t& x)
{
	vector_t y;
	float X = x.x, Y = x.y, Z = x.z, W = x.w;
	y.x = X * m.m[0][0] + Y * m.m[1][0] + Z * m.m[2][0] + W * m.m[3][0];
	y.y = X * m.m[0][1] + Y * m.m[1][1] + Z * m.m[2][1] + W * m.m[3][1];
	y.z = X * m.m[0][2] + Y * m.m[1][2] + Z * m.m[2][2] + W * m.m[3][2];
	y.w = X * m.m[0][3] + Y * m.m[1][3] + Z * m.m[2][3] + W * m.m[3][3];
	return y;
}

vector_t operator*(const matrix_t& m, const vector_t& x)
{
	return matrix_apply(m,x);
}

matrix_t matrix_translate(const matrix_t& m, const vector_t& v)
{
	matrix_t z = m;
	z.m[3][0] = v.x;
	z.m[3][1] = v.y;
	z.m[3][2] = v.z;
	return z;
}

matrix_t matrix_rotate(const matrix_t& m, float angle, const vector_t& v)
{
	float x = v.x, y = v.y, z = v.z;
	float qsin = (float)sin(angle * 0.5f);
	float qcos = (float)cos(angle * 0.5f);
	vector_t vec = { x, y, z, 1.0f };
	float w = qcos;
	vec = vector_normalize(vec);
	x = vec.x * qsin;
	y = vec.y * qsin;
	z = vec.z * qsin;
	matrix_t m_ret;
	m_ret.m[0][0] = 1 - 2 * y * y - 2 * z * z;
	m_ret.m[1][0] = 2 * x * y - 2 * w * z;
	m_ret.m[2][0] = 2 * x * z + 2 * w * y;
	m_ret.m[0][1] = 2 * x * y + 2 * w * z;
	m_ret.m[1][1] = 1 - 2 * x * x - 2 * z * z;
	m_ret.m[2][1] = 2 * y * z - 2 * w * x;
	m_ret.m[0][2] = 2 * x * z - 2 * w * y;
	m_ret.m[1][2] = 2 * y * z + 2 * w * x;
	m_ret.m[2][2] = 1 - 2 * x * x - 2 * y * y;
	m_ret.m[0][3] = m_ret.m[1][3] = m_ret.m[2][3] = 0.0f;
	m_ret.m[3][0] = m_ret.m[3][1] = m_ret.m[3][2] = 0.0f;
	m_ret.m[3][3] = 1.0f;
	return m_ret;
}

vector_t transform_apply(const transform_t& ts, const vector_t& x)
{
	return (ts.transform)*(x);
}

vector_t operator*(const transform_t& ts, const vector_t& x)
{
	return transform_apply(ts,x);
}

//return x-y
vector_t vector_sub(const vector_t& x, const vector_t& y)
{
	vector_t z;
	z.x = x.x - y.x;
	z.y = x.y - y.y;
	z.z = x.z - y.z;
	z.w = 1.0;
	return z;
}

vector_t operator-(const vector_t& x, const vector_t& y)
{
	return vector_sub(x,y);
}

vector_t vector_normalize(const vector_t& v)
{
	vector_t z=v;
	float length = vector_length(v);
	if (length != 0.0f) {
		float inv = 1.0f / length;
		z.x *= inv;
		z.y *= inv;
		z.z *= inv;
	}
	return z;
}

float vector_length(const vector_t& v)
{
	float sq = v.x * v.x + v.y * v.y + v.z * v.z;
	return (float)sqrt(sq);
}

vector_t vector_cross(const vector_t& x, const vector_t& y)
{
	vector_t z;
	z.x = x.y * y.z - x.z * y.y;
	z.y = x.z * y.x - x.x * y.z;
	z.z = x.x * y.y - x.y * y.x;
	z.w = 1.0f;
	return z;
}

float vector_dotProduct(const vector_t& x, const vector_t& y)
{
	return x.x * y.x + x.y * y.y + x.z * y.z;;
}

float operator*(const vector_t& x, const vector_t& y)
{
	return vector_dotProduct(x,y);
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
	matrix_set_identity(&ts->model);
	matrix_set_identity(&ts->view);
	matrix_set_perspective(&ts->projection, PI * 0.5f, aspect, 1.0f, 500.0f);
	ts->w = (float)width;
	ts->h = (float)height;
	ts->transform = ts->projection * ts->view * ts->model;//update
}

vector_t viewport_transform(const vector_t& x, const transform_t& ts)
{
	vector_t y;
	float rhw = 1.0f / x.w;
	y.x = (x.x * rhw + 1.0f) * ts.w * 0.5f;
	y.y = (1.0f - x.y * rhw) * ts.h * 0.5f;
	y.z = x.z * rhw;
	y.w = 1.0f;
	return y;
}

int check_cvv(const vector_t& v)
{
	float w = v.w;
	int check = 0;
	if (v.z < 0.0f) check |= 1;
	if (v.z > w) check |= 2;
	if (v.x < -w) check |= 4;
	if (v.x > w) check |= 8;
	if (v.y < -w) check |= 16;
	if (v.y > w) check |= 32;
	return check;
}
