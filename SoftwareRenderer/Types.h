#pragma once
#include"Includes.h"

#define PI 3.1415926f

typedef unsigned int UINT32;

typedef struct { float m[4][4]; } matrix_t;
typedef struct { float x, y, z, w; } vector_t;
typedef vector_t point_t;
typedef struct { float r, g, b, a; } color_t;
typedef struct { float u, v; } texcoord_t;

typedef struct { point_t pos; color_t color; texcoord_t tex; float rhw; } vertex_t;

typedef struct {
	matrix_t model;         // ��������任
	matrix_t view;          // ��Ӱ������任
	matrix_t projection;    // ͶӰ�任
	matrix_t transform;     // transform = model * view * projection;
	float w, h;             // ��Ļ��С
}	transform_t;