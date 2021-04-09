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
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			z.m[i][j] = (left.m[i][0] * right.m[0][j]) +
				(left.m[i][1] * right.m[1][j]) +
				(left.m[i][2] * right.m[2][j]) +
				(left.m[i][3] * right.m[3][j]);
		}
	}
	return z;
}
matrix_t operator*(const matrix_t& left, const matrix_t& right)
{
	return matrix_mul(left, right);
}

vector_t matrix_apply(const vector_t& x, const matrix_t& m)
{
	vector_t y;
	float X = x.x, Y = x.y, Z = x.z, W = x.w;
	y.x = X * m.m[0][0] + Y * m.m[1][0] + Z * m.m[2][0] + W * m.m[3][0];
	y.y = X * m.m[0][1] + Y * m.m[1][1] + Z * m.m[2][1] + W * m.m[3][1];
	y.z = X * m.m[0][2] + Y * m.m[1][2] + Z * m.m[2][2] + W * m.m[3][2];
	y.w = X * m.m[0][3] + Y * m.m[1][3] + Z * m.m[2][3] + W * m.m[3][3];
	return y;
}

vector_t operator*(const vector_t& x, const matrix_t& m)
{
	return matrix_apply(x,m);
}

matrix_t matrix_translate_build(float x, float y, float z)
{
	matrix_t m_ret;
	m_ret.m[3][0] = x;
	m_ret.m[3][1] = y;
	m_ret.m[3][2] = z;
	return m_ret;
}

matrix_t matrix_rotate_build(float angle, const vector_t& v)
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

//return (matrix)T
matrix_t matrix_transpose(const matrix_t& m)
{
	matrix_t z;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			z.m[i][j] = m.m[j][i];
		}
	}
	return z;
}

matrix_t matrix_scale(const matrix_t& m, const vector_t& v)
{
	matrix_t y = m;
	y.m[0][0] *= v.x;
	y.m[1][1] *= v.y;
	y.m[2][2] *= v.z;
	return y;
}

matrix_t matrix_translate(const matrix_t& m, const vector_t& v)
{
	matrix_t y = m;
	y.m[3][0] += v.x;
	y.m[3][1] += v.y;
	y.m[3][2] += v.z;
	return y;
}

void matrix_set_identity(matrix_t* m) {
	m->m[0][0] = m->m[1][1] = m->m[2][2] = m->m[3][3] = 1.0f;
	m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
	m->m[1][0] = m->m[1][2] = m->m[1][3] = 0.0f;
	m->m[2][0] = m->m[2][1] = m->m[2][3] = 0.0f;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0f;
}

void matrix_set_perspective(matrix_t* m, float fovy, float aspect, float z_near, float z_far) {
	float fax = 1.0f / (float)tan(fovy * 0.5f);
	matrix_set_zero(m);
	//matrix_set_identity(m);//会导致裁剪失效
	m->m[0][0] = (float)(fax / aspect);
	m->m[1][1] = (float)(fax);
	m->m[2][2] = z_far / (z_far - z_near);
	m->m[2][3] = 1;	// 将z值储存在w中
	m->m[3][2] = -z_near * z_far / (z_far - z_near);
}

vector_t transform_apply(const vector_t& x, const transform_t& ts)
{
	return (x)*(ts.transform);
}

vector_t operator*(const vector_t& x, const transform_t& ts)
{
	return transform_apply(x,ts);
}

vector_t vector_add(const vector_t& x, const vector_t& y)
{
	vector_t z;
	z.x = x.x + y.x;
	z.y = x.y + y.y;
	z.z = x.z + y.z;
	z.w = 1.0;
	return z;
}

vector_t operator+(const vector_t& x, const vector_t& y)
{
	return vector_add(x, y);
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

const vector_t operator-(const vector_t& x)
{
	return (x * -1);
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

float vector_dot(const vector_t& x, const vector_t& y)
{
	return x.x * y.x + x.y * y.y + x.z * y.z;;
}

vector_t operator*(const vector_t& x, const float y)
{
	vector_t z = x;
	z.x *= y;
	z.y *= y;
	z.z *= y;
	return z;
}

vector_t operator*(const float y, const vector_t& x)
{
	vector_t z = x;
	z.x *= y;
	z.y *= y;
	z.z *= y;
	return z;
}

//I为从光源指向目标的方向向量,n为顶点法向量.返回以目标为起点的反射光向量
vector_t vector_reflect(const vector_t& I, const vector_t& n)
{
	vector_t N = vector_normalize(n) * vector_dot(N, I);
	vector_t I_ref = (I - N) - N;

	return I_ref;
}

void vertex_set_rhw(vertex_t* v)
{
	float rhw = 1.0f / v->pos.w;
	v->rhw = rhw;
	v->tex.u *= rhw;
	v->tex.v *= rhw;
	v->color.r *= rhw;
	v->color.g *= rhw;
	v->color.b *= rhw;
	v->color.a *= rhw;
}

void transform_init(transform_t* ts, int width, int height) {
	float aspect = (float)width / ((float)height);
	matrix_set_identity(&ts->model);
	matrix_set_identity(&ts->view);
	matrix_set_perspective(&ts->projection, 45.0f, aspect, 0.1f, 100.0f);
	ts->w = (float)width;
	ts->h = (float)height;
	ts->transform = ts->model * ts->view * ts->projection;//update
}

color_t color_add(const color_t& x, const color_t& y)
{
	color_t z;
	z.r = x.r + y.r;
	z.g = x.g + y.g;
	z.b = x.b + y.b;
	z.a = x.a + y.a;
	return z;
}

color_t operator+(const color_t& x, const color_t& y)
{
	return color_add(x, y);
}

color_t color_mul_num(const color_t& x, const float& y)
{
	color_t z;
	z.r = x.r * y;
	z.g = x.g * y;
	z.b = x.b * y;
	z.a = x.a * y;
	return z;
}

color_t operator*(const color_t& x, const float& y)
{
	return color_mul_num(x, y);
}

color_t operator*(const float& y, const color_t& x)
{
	return color_mul_num(x, y);
}

void operator*=(color_t& x, const float y)
{
	x = x * y;
}

color_t color_mul_color(const color_t& x, const color_t& y)
{
	color_t z;
	z.r = x.r * y.r;
	z.g = x.g * y.g;
	z.b = x.b * y.b;
	z.a = x.a * y.a;
	return z;
}

color_t operator*(const color_t& x, const color_t& y)
{
	return color_mul_color(x, y);
}

color_t color_sub(const color_t& x, const color_t& y)
{
	color_t z;
	z.r = x.r - y.r;
	z.g = x.g - y.g;
	z.b = x.b - y.b;
	z.a = x.a - y.a;
	return z;
}

color_t operator-(const color_t& x, const color_t& y)
{
	return color_sub(x, y);
}

const color_t operator - (const color_t& x)
{
	return (x * -1);
}

color_t color_div(const color_t& x, const float& y)
{
	color_t z;
	z.r = x.r / y;
	z.g = x.g / y;
	z.b = x.b / y;
	z.a = x.a / y;
	return z;
}

color_t operator/(const color_t& x, const float& y)
{
	return color_div(x, y);
}

float time_get()
{
	auto current_time = std::chrono::steady_clock::now();
	auto duration_in_seconds = std::chrono::duration<double>(current_time.time_since_epoch());
	float t = duration_in_seconds.count();
	return t;
}

vector_t viewport_transform(const vector_t& x, const transform_t& ts)
{
	vector_t y;
	float rhw = 1.0f / x.w;
	y.x = (x.x * rhw + 1.0f) * ts.w * 0.5f;
	y.y = (1.0f - x.y * rhw) * ts.h * 0.5f;
	y.z = x.z * rhw;
	y.w = x.w;
	return y;
}

int check_cvv(const vector_t& v)
{
	float w = v.w;
	int check = 0;
	// 进行projection映射后,x/y都是x*z/y*z的形式,在projection以后x/y应该在[-1,1]之间,-
	// -现在*z,所以x/y应该在[-z,z]之间,注意这里的z都是指projection以前的z值,即现在的w.
	if (v.z < 0.0f) check |= 1;
	if (v.z > w) check |= 2;
	if (v.x < -w) check |= 4;
	if (v.x > w) check |= 8;
	if (v.y < -w) check |= 16;
	if (v.y > w) check |= 32;
	return check;
}

float interp(float length_total, float length_place, float x1, float x2)
{
	float t = length_place / length_total;
	return (x1 + (x2 - x1) * t);
}

