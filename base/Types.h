#pragma once
#include"Includes.h"

#define PI				3.1415926535898f
#define KEY_A                  65
#define KEY_B                  66
#define KEY_C                  67
#define KEY_D                  68
#define KEY_E                  69
#define KEY_F                  70
#define KEY_G                  71
#define KEY_H                  72
#define KEY_I                  73
#define KEY_J                  74
#define KEY_K                  75
#define KEY_L                  76
#define KEY_M                  77
#define KEY_N                  78
#define KEY_O                  79
#define KEY_P                  80
#define KEY_Q                  81
#define KEY_R                  82
#define KEY_S                  83
#define KEY_T                  84
#define KEY_U                  85
#define KEY_V                  86
#define KEY_W                  87
#define KEY_X                  88
#define KEY_Y                  89
#define KEY_Z                  90

struct color_t;

typedef unsigned int UINT32;

struct color_t {
	float r, g, b, a;
	color_t() :r(0), g(0), b(0), a(1) {  }
	color_t(float r, float g, float b, float a) :r(r), g(g), b(b), a(a) {  }
	color_t(float r) :r(r), g(r), b(r), a(r) {  }
};

typedef struct { float m[4][4]; } matrix_t;

struct vector_t{ 
	float x, y, z, w;
	vector_t() :x(0), y(0), z(0), w(0) {}
	vector_t(float x, float y, float z, float w) :x(x), y(y), z(z), w(w) {}
	vector_t(float x, float y, float z) :x(x), y(y), z(z), w(1) {}
	vector_t(float x) :x(x), y(x), z(x), w(x) {}
	void operator=(const color_t& c) { x = c.r; y = c.g; z = c.b; w = c.a; }
	vector_t(const color_t& c) :x(c.r), y(c.g), z(c.b), w(c.a) { }
};

struct ray_t { 
	vector_t o, dir; 
	ray_t(vector_t o = vector_t(), vector_t dir = vector_t()) :o(o), dir(dir) {}
};
using point_t = vector_t;

struct texcoord_t {
	float u, v;
	texcoord_t() :u(0), v(0) {}
	texcoord_t(float u, float v) :u(u), v(v) {}
	texcoord_t operator-(const texcoord_t& x) { return texcoord_t(u - x.u, v - x.v); }
	texcoord_t operator+(const texcoord_t& x) { return texcoord_t(u + x.u, v + x.v); }
	texcoord_t operator*(float x) { return texcoord_t(u * x, v * x); }
};
typedef struct { float w1, w2, w3; } barycentric_t;

struct material_t{
	enum class reflect_t {
		DIFF, //diffuse漫反射
		SPEC, //specular反射
		REFR  //refraction折射
	};

	reflect_t reflect = reflect_t::DIFF;
};

struct vertex_t { 
	point_t pos; 
	point_t pos_world;
	point_t pos_model;
	color_t color; 
	texcoord_t tex; 
	vector_t normal;
	float rhw; 

	//For PBR
	color_t emissivity;
	material_t material;
};

struct triangle_t {
	vertex_t v1, v2, v3;
};

typedef struct {
	matrix_t model;         // 世界坐标变换
	matrix_t view;          // 摄影机坐标变换
	matrix_t projection;    // 投影变换
	matrix_t transform;     // transform = model * view * projection;
	matrix_t TBN;
	float w, h;             // 屏幕大小
}	transform_t;