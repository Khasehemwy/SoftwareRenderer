#pragma once
#include"Includes.h"

class Renderer_Skybox :public Renderer {
public:
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

	return texture_cube->Read(tex_coord);
}

class Renderer_PBR :public Renderer {
	texcoord_t ParallaxMapping(texcoord_t texCoords, vector_t viewDir);
	vector_t FresnelSchlick(float cosTheta, vector_t F0);
	float DistributionGGX(vector_t N, vector_t H, float roughness);
	float GeometrySchlickGGX(float NdotV, float roughness);
	float GeometrySmith(vector_t N, vector_t V, vector_t L, float roughness);
public:
	point_t view_pos;
	virtual void VS(vertex_t* v1, vertex_t* v2, vertex_t* v3);
	virtual color_t PS(vertex_t* v);
};

inline texcoord_t Renderer_PBR::ParallaxMapping(texcoord_t texCoords, vector_t viewDir)
{
	viewDir = viewDir * matrix_transpose(transform.TBN);
	viewDir = vector_normalize(viewDir);

	const float minLayers = 8;
	const float maxLayers = 32;
	float numLayers = mix(maxLayers, minLayers, abs(vector_dot(vector_t(0.0, 0.0, 1.0), viewDir)));

	float layerDepth = 1.0 / numLayers;

	float currentLayerDepth = 0.0;

	float heightScale = 0.1;

	vector_t P = viewDir / viewDir.z * heightScale;
	vector_t deltaTexCoordsVec = P / numLayers;
	texcoord_t deltaTexCoords(deltaTexCoordsVec.x, deltaTexCoordsVec.y);

	texcoord_t currentTexCoords = texCoords;
	float currentDepthMapValue = 1.0f - textures["height"]->Read(currentTexCoords.u, currentTexCoords.v).r;

	while (currentLayerDepth < currentDepthMapValue)
	{
		currentTexCoords = currentTexCoords - deltaTexCoords;

		currentDepthMapValue = 1.0f - textures["height"]->Read(currentTexCoords.u, currentTexCoords.v).r;
		
		currentLayerDepth += layerDepth;
	}

	texcoord_t prevTexCoords = currentTexCoords + deltaTexCoords;

	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = 1.0f - textures["height"]->Read(prevTexCoords.u, prevTexCoords.v).r - currentLayerDepth + layerDepth;

	float weight = afterDepth / (afterDepth - beforeDepth);
	texcoord_t finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}

inline vector_t Renderer_PBR::FresnelSchlick(float cosTheta, vector_t F0)
{
	return F0 + (1.0 - F0) * pow(CMID(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

inline float Renderer_PBR::DistributionGGX(vector_t N, vector_t H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(vector_dot(N, H), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return (nom / denom);
}

inline float Renderer_PBR::GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return (nom / denom);
}

inline float Renderer_PBR::GeometrySmith(vector_t N, vector_t V, vector_t L, float roughness)
{
	float NdotV = max(vector_dot(N, V), 0.0f);
	float NdotL = max(vector_dot(N, L), 0.0f);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return (ggx1 * ggx2);
}

void Renderer_PBR::VS(vertex_t* v1, vertex_t* v2, vertex_t* v3)
{
	v1->pos = v1->pos * this->transform.model;
	v2->pos = v2->pos * this->transform.model;
	v3->pos = v3->pos * this->transform.model;

	v1->pos_world = v1->pos;
	v2->pos_world = v2->pos;
	v3->pos_world = v3->pos;

	//计算TBN矩阵
	{
		vector_t edge1 = v2->pos - v1->pos;
		vector_t edge2 = v3->pos - v1->pos;
		vector_t deltaUV1(v2->tex.u - v1->tex.u, v2->tex.v - v1->tex.v, 0, 1);
		vector_t deltaUV2(v3->tex.u - v1->tex.u, v3->tex.v - v1->tex.v, 0, 1);

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		vector_t T;
		T.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		T.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		T.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		T = vector_normalize(T);

		vector_t N = vector_normalize(vector_cross(v3->pos - v1->pos, v2->pos - v1->pos));

		vector_t B = vector_normalize(vector_cross(T, N));

		transform.TBN.m[0][0] = T.x;
		transform.TBN.m[0][1] = T.y;
		transform.TBN.m[0][2] = T.z;
		transform.TBN.m[0][3] = 0;

		transform.TBN.m[1][0] = B.x;
		transform.TBN.m[1][1] = B.y;
		transform.TBN.m[1][2] = B.z;
		transform.TBN.m[1][3] = 0;

		transform.TBN.m[2][0] = N.x;
		transform.TBN.m[2][1] = N.y;
		transform.TBN.m[2][2] = N.z;
		transform.TBN.m[2][3] = 0;

		transform.TBN.m[3][3] = 1;
	}
	//

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
	vector_t V = vector_normalize(view_pos - v->pos_world);

	//if (textures["height"]) {
	//	v->tex = ParallaxMapping(v->tex, V);

	//	if (v->tex.u > 1 || v->tex.u < 0 || v->tex.v > 1 || v->tex.v < 0) {
	//		return color_trans_1f(background);
	//	}
	//}
	//vector_t height = textures["height"]->Read(v->tex.u, v->tex.v).r;

	vector_t N = vector_normalize((textures["normal"]->Read(v->tex.u, v->tex.v) * 2 - vector_t(1)) * this->transform.TBN);
	//vector_t N = vector_normalize(textures["normal"]->Read(v->tex.u, v->tex.v) * this->transform.TBN);
	//vector_t N = v->normal;
	//N = N * 0.5f + vector_t(0.5);

	vector_t F0 = vector_t(0.04);
	vector_t albedo = textures["diffuse"]->Read(v->tex.u, v->tex.v);
	//vector_t albedo(0.5f, 0.0f, 0.0f);
	float metallic = textures["metallic"]->Read(v->tex.u, v->tex.v).r;
	//float metallic = 0.5;
	F0 = mix(F0, albedo, metallic);

	vector_t Lo = vector_t(0.0);
	for (int i = 0; i < lights.size(); i++) {
		const Light* curLight = lights[i];

		vector_t L = vector_normalize(curLight->pos - v->pos_world);
		vector_t H = vector_normalize(V + L);

		float distance = vector_length(curLight->pos- v->pos_world);
		vector_t radiance = curLight->radiance * 1.0f / (distance * distance);

		vector_t F = FresnelSchlick(CMID(vector_dot(H, V), 0.0, 1.0), F0);
		float roughness = textures["roughness"]->Read(v->tex.u, v->tex.v).r;
		//float roughness = 0.1;
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);

		vector_t numerator = NDF * G * F;
		float denominator = 4.0 * max(vector_dot(N, V), 0.0f) * max(vector_dot(N, L), 0.0f) + 0.0001;
		vector_t specular = numerator / denominator;

		vector_t kS = F;
		vector_t kD = vector_t(1.0) - kS;
		kD = kD * (1.0 - metallic);

		float NdotL = max(vector_dot(N, L), 0.0f);
		Lo = Lo + (kD * albedo / PI + specular) * radiance * NdotL;
	}

	float ao = textures["ao"]->Read(v->tex.u, v->tex.v).r;
	//float ao = 1;
	vector_t ambient = vector_dot(vector_t(0.2) , albedo) * ao;
	vector_t color = Lo + ambient;

	color = color / (color + vector_t(1.0));

	//float gamma = 2.2;
	//color.x = pow(color.x, 1.0f / gamma);
	//color.y = pow(color.y, 1.0f / gamma);
	//color.z = pow(color.z, 1.0f / gamma);

	color_t color_use;
	color_use.r = color.x;
	color_use.g = color.y;
	color_use.b = color.z;
	color_use.a = 1;

	//color_use.r = N.x * 0.5 + 0.5;
	//color_use.g = N.y * 0.5 + 0.5;
	//color_use.b = N.z * 0.5 + 0.5;
	//color_use.a = 1;

	//color_use.r = albedo.x;
	//color_use.g = albedo.y;
	//color_use.b = albedo.z;
	//color_use.a = 1;

	return color_use;
}