#pragma once
#include"Includes.h"

class Light {
public:
	std::vector<vertex_t>* vertexs;

	vector_t pos;

	color_t ambient;
	color_t diffuse;
	color_t specular;
	float intensity;	//π‚’’«ø∂»

	void set_vertex();
};