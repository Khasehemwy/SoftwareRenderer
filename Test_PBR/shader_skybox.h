#pragma once
#include"Includes.h"

class Renderer_Skybox :public Renderer {
public:
	bool isHDR = false;
	float exposure = 1.0f;
	float gamma = 2.2f;

	Texture_Cube* texture_cube;
	virtual void VS(vertex_t* v1, vertex_t* v2, vertex_t* v3);
	virtual color_t PS(vertex_t* v);
};

void Renderer_Skybox::VS(vertex_t* v1, vertex_t* v2, vertex_t* v3)
{
	v1->pos_model = v1->pos;
	v2->pos_model = v2->pos;
	v3->pos_model = v3->pos;

	v1->pos = v1->pos * this->transform.model;
	v2->pos = v2->pos * this->transform.model;
	v3->pos = v3->pos * this->transform.model;

	v1->pos_world = v1->pos;
	v2->pos_world = v2->pos;
	v3->pos_world = v3->pos;

	v1->pos = v1->pos * this->transform.view;
	v2->pos = v2->pos * this->transform.view;
	v3->pos = v3->pos * this->transform.view;

	v1->pos = v1->pos * this->transform.projection;
	v2->pos = v2->pos * this->transform.projection;
	v3->pos = v3->pos * this->transform.projection;
}

inline color_t Renderer_Skybox::PS(vertex_t* v)
{
	vector_t tex_coord = v->pos_model;
	tex_coord.y = -tex_coord.y;

	//texture_cube->Write(tex_coord, color_t(1.0));
	vector_t color = texture_cube->Read(tex_coord);

	color.x = pow(color.x, gamma);
	color.y = pow(color.y, gamma);
	color.z = pow(color.z, gamma);

	if (isHDR) {
		//color = vector_t(1.0) - 
		//	vector_t(exp(-color.x * exposure),
		//		exp(-color.y * exposure), 
		//		exp(-color.z * exposure));

		color = color / (color + vector_t(1.0f));
	}

	color.x = pow(color.x, 1.0f / gamma);
	color.y = pow(color.y, 1.0f / gamma);
	color.z = pow(color.z, 1.0f / gamma);

	color_t color_use(color.x, color.y, color.z, 1);
	return color_use;
}