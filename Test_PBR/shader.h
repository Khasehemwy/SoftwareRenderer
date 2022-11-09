#pragma once
#include"Includes.h"

class Renderer_PBR :public Renderer {
public:
	virtual void VS(vertex_t* v1, vertex_t* v2, vertex_t* v3);
	virtual color_t PS(vertex_t* v);
};

void Renderer_PBR::VS(vertex_t* v1, vertex_t* v2, vertex_t* v3)
{
	if (this->render_state == RENDER_STATE_TEXTURE) {
		v1->color = v2->color = v3->color = { 1,1,1,1 };
	}

	v1->pos = v1->pos * this->transform.model;
	v2->pos = v2->pos * this->transform.model;
	v3->pos = v3->pos * this->transform.model;

	v1->pos_world = v1->pos;
	v2->pos_world = v2->pos;
	v3->pos_world = v3->pos;

	v1->normal = v1->normal * transform.model * transform.view;
	v2->normal = v2->normal * transform.model * transform.view;
	v3->normal = v3->normal * transform.model * transform.view;

	/* 将点映射到观察空间 */
	v1->pos = v1->pos * this->transform.view;
	v2->pos = v2->pos * this->transform.view;
	v3->pos = v3->pos * this->transform.view;

	/* 将点映射到裁剪空间 */
	v1->pos = v1->pos * this->transform.projection;
	v2->pos = v2->pos * this->transform.projection;
	v3->pos = v3->pos * this->transform.projection;
}

color_t Renderer_PBR::PS(vertex_t* v)
{
	color_t color_use;

	color_use = textures["diffuce"]->Read(v->tex.u, v->tex.v);

	return color_use;
}