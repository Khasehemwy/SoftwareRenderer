#pragma once
#include"Includes.h"

class Light {
public:
	std::vector<vertex_t>* vertexs;

	//玩具光源
	vector_t pos;
	color_t color;
	float intensity;	//光照强度

	void set_vertex();
};