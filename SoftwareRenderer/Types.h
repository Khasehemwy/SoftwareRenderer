#pragma once
#include"Includes.h"

typedef unsigned int UINT32;

typedef struct { float m[4][4]; } matrix_t;
typedef struct { float x, y, z, w; } vector_t;
typedef vector_t point_t;
typedef struct { float r, g, b, a; } color_t;

typedef struct { point_t pos; color_t color; float rhw; } vertex_t;

typedef struct {
	matrix_t world;         // ��������任
	matrix_t view;          // ��Ӱ������任
	matrix_t projection;    // ͶӰ�任
	matrix_t transform;     // transform = world * view * projection;
	float w, h;             // ��Ļ��С
}	transform_t;