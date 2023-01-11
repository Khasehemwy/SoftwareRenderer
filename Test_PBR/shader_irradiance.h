#pragma once
#include"Includes.h"

class Renderer_Irradiance :public Renderer {
public:
	Texture_Cube* texture_cube;
	Texture_Cube* texture_cube_irradiance;

	virtual void VS(vertex_t* v1, vertex_t* v2, vertex_t* v3);
	virtual color_t PS(vertex_t* v);
};

void Renderer_Irradiance::VS(vertex_t* v1, vertex_t* v2, vertex_t* v3)
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

inline color_t Renderer_Irradiance::PS(vertex_t* v)
{
	vector_t irradiance = vector_t(0.0);

	vector_t N = vector_normalize(v->pos_world);
	vector_t up = vector_t(0.0, 1.0, 0.0);
	vector_t right = vector_cross(up, N);
	up = vector_cross(N, right);

	float sampleDelta = 1.0;
	float nrSamples = 0.0;
	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vector_t tangentSample = vector_t(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vector_t sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
			sampleVec.y = -sampleVec.y;

			color_t sampleColor = texture_cube->Read(sampleVec) * cos(theta) * sin(theta);
			irradiance = irradiance + sampleColor;
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	vector_t tex_coord = v->pos_model;
	tex_coord.y = -tex_coord.y;

	color_t color_irradiance;
	color_irradiance.r = irradiance.x;
	color_irradiance.g = irradiance.y;
	color_irradiance.b = irradiance.z;
	color_irradiance.a = 1;
	this->texture_cube_irradiance->Write(tex_coord, color_irradiance);

	//vector_t tex_coord = v->pos_model;
	//tex_coord.y = -tex_coord.y;

	//texture_cube_irradiance->Write(tex_coord, color_t(1.0));

	return color_irradiance;
}