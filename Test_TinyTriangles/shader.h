#pragma once
#include"Includes.h"

class Renderer_Rasterization :public Renderer {
private:
public:
	unsigned int primitive_id = 0;
	virtual void VS(vertex_t* v1, vertex_t* v2, vertex_t* v3);
	virtual color_t PS(vertex_t* v);
};

void Renderer_Rasterization::VS(vertex_t* v1, vertex_t* v2, vertex_t* v3)
{
	primitive_id++;

	v1->pos_model = v1->pos;
	v2->pos_model = v2->pos;
	v3->pos_model = v3->pos;

	v1->pos = v1->pos * this->transform.model;
	v2->pos = v2->pos * this->transform.model;
	v3->pos = v3->pos * this->transform.model;

	v1->pos = v1->pos * this->transform.view;
	v2->pos = v2->pos * this->transform.view;
	v3->pos = v3->pos * this->transform.view;

	v1->pos = v1->pos * this->transform.projection;
	v2->pos = v2->pos * this->transform.projection;
	v3->pos = v3->pos * this->transform.projection;

	v1->color.r = 1.0f / ((primitive_id * 29819)%100) * 10;
	v1->color.g = 1.0f / ((primitive_id * 918291)%100) * 10;
	v1->color.b = 1.0f / ((primitive_id * 81941)%100)* 10;
}

inline color_t Renderer_Rasterization::PS(vertex_t* v)
{
	return v->color;
}