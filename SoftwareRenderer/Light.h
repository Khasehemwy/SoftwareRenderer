#pragma once
#include"Includes.h"

class Light {
public:
	std::vector<vertex_t>* vertexs;

	//��߹�Դ
	vector_t pos;
	color_t color;
	float intensity;	//����ǿ��

	void set_vertex();
};