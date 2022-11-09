﻿#pragma once
#include"Includes.h"

#define LIGHT_STATE_DIRECTIONAL 0b1 
#define LIGHT_STATE_POINT 0b10 
#define LIGHT_STATE_SPOTLIGHT 0b100

class Light {
public:
	vector_t pos;
	vector_t direction;	//从光出发的向量

	color_t ambient;
	color_t diffuse;
	color_t specular;

	//PBR
	color_t radiance;

	//阴影
	matrix_t light_space_matrix;	//view * projection
	Texture* shadow_map = nullptr;	// 阴影贴图

	//点光源所用
	float constant = 1.0f;
	float linear = 0.045f;
	float quadratic = 0.0075f;

	//聚光所用
	float cut_off;
	float outer_cut_off;

	int light_state = 0x10;

	Light();

	void Init_ShadowMap(int width, int height);
	void Set_ShadowMap(const Renderer& renderer);
};