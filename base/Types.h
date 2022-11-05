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

typedef unsigned int UINT32;

typedef struct { float m[4][4]; } matrix_t;
struct vector_t{ 
	float x, y, z, w;
	vector_t(float x = 0, float y = 0, float z = 0, float w = 0) :x(x), y(y), z(z), w(w) {}
};
struct ray_t { 
	vector_t o, dir; 
	ray_t(vector_t o = vector_t(), vector_t dir = vector_t()) :o(o), dir(dir) {}
};
using point_t = vector_t;
struct color_t{
	float r, g, b, a;
	color_t(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f)
		:r(r), g(g), b(b), a(a) {  }
};
typedef struct { float u, v; } texcoord_t;
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
	float w, h;             // 屏幕大小
}	transform_t;

struct Draw_ExtraData
{
	struct World_Pos
	{
		//阴影需要用的三个顶点的世界坐标,用于光栅化时插值计算出每个像素在光源空间的坐标
		bool valid = false;
		point_t p1, p2, p3;
	};

	bool valid = false;
	World_Pos world_pos;
};