#pragma once
#include"Includes.h"

#define LIGHT_STATE_DIRECTIONAL 0x1 
#define LIGHT_STATE_POINT 0x10 
#define LIGHT_STATE_SPOTLIGHT 0x100

class Light {
public:
	vector_t pos;
	vector_t direction;	//从光出发的向量

	color_t ambient;
	color_t diffuse;
	color_t specular;

	//点光源所用
	float constant = 1.0f;
	float linear = 0.045f;
	float quadratic = 0.0075f;

	//聚光所用
	float cut_off;
	float outer_cut_off;

	int light_state = 0x10;

	Light();
};