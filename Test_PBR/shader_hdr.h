#pragma once
#include"Includes.h"

class Renderer_PBR :public Renderer {
	const float MAX_REFLECTION_LOD = 4.0;

	texcoord_t ParallaxMapping(texcoord_t texCoords, vector_t viewDir);
	vector_t fresnelSchlickRoughness(float cosTheta, vector_t F0, float roughness);
	vector_t fresnelSchlick(float cosTheta, vector_t F0);
	float DistributionGGX(vector_t N, vector_t H, float roughness);
	float GeometrySchlickGGX(float NdotV, float roughness);
	float GeometrySmith(vector_t N, vector_t V, vector_t L, float roughness);
public:
	bool enable_gamma = true;
	bool enable_height_texture = false;
	bool enable_hdr = true;
	bool only_albedo = false;

	float gamma = 2.2;
	point_t view_pos;
	std::unordered_map<std::string, Texture_Cube*> cube_textures;

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

inline vector_t Renderer_PBR::fresnelSchlickRoughness(float cosTheta, vector_t F0, float roughness)
{
	return F0 + (vector_t(max(1 - roughness, F0.x), max(1 - roughness, F0.y), max(1 - roughness, F0.z)) - F0) * pow(CMID(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

inline vector_t Renderer_PBR::fresnelSchlick(float cosTheta, vector_t F0)
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

	if (enable_height_texture && textures["height"]) {
		v->tex = ParallaxMapping(v->tex, V);

		if (v->tex.u > 1 || v->tex.u < 0 || v->tex.v > 1 || v->tex.v < 0) {
			return color_t(0, 0, 0, 0);
		}
	}
	//vector_t height = textures["height"]->Read(v->tex.u, v->tex.v).r;

	vector_t N = vector_normalize((textures["normal"]->Read(v->tex.u, v->tex.v) * 2 - vector_t(1)) * this->transform.TBN);
	//vector_t N = vector_normalize(textures["normal"]->Read(v->tex.u, v->tex.v) * this->transform.TBN);
	//vector_t N = v->normal;
	//N = N * 0.5f + vector_t(0.5);

	vector_t R = vector_reflect(-V, N);


	// Gamma correction
	vector_t albedo = textures["diffuse"]->Read(v->tex.u, v->tex.v);
	//vector_t albedo(0.5f, 0.0f, 0.0f);
	if (enable_gamma) {
		albedo.x = pow(albedo.x, gamma);
		albedo.y = pow(albedo.y, gamma);
		albedo.z = pow(albedo.z, gamma);
	}

	if (only_albedo) {
		color_t color;
		color.r = albedo.x;
		color.g = albedo.y;
		color.b = albedo.z;
		color.a = 1;

		if (enable_gamma)
		{
			color.r = pow(color.r, 1.0f / gamma);
			color.g = pow(color.g, 1.0f / gamma);
			color.b = pow(color.b, 1.0f / gamma);
		}

		return color;
	}

	float metallic = textures["metallic"]->Read(v->tex.u, v->tex.v).r;
	//float metallic = 0;
	float roughness = textures["roughness"]->Read(v->tex.u, v->tex.v).r;

	float ao = textures["ao"]->Read(v->tex.u, v->tex.v).r;
	//float ao = 1;

	vector_t F0 = vector_t(0.04);
	F0 = mix(F0, albedo, metallic);

	int prefilteredLod = round(roughness * MAX_REFLECTION_LOD);
	vector_t prefilteredColor;
	switch (prefilteredLod)
	{
	case 0:
		prefilteredColor = cube_textures["environment0"]->Read({ R.x,-R.y,R.z,1 });
		break;

	case 1:
		prefilteredColor = cube_textures["environment1"]->Read({ R.x,-R.y,R.z,1 });
		break;

	case 2:
		prefilteredColor = cube_textures["environment2"]->Read({ R.x,-R.y,R.z,1 });
		break;

	case 3:
		prefilteredColor = cube_textures["environment3"]->Read({ R.x,-R.y,R.z,1 });
		break;

	case 4:
		prefilteredColor = cube_textures["environment4"]->Read({ R.x,-R.y,R.z,1 });
		break;

	default:
		prefilteredColor = cube_textures["environment0"]->Read({ R.x,-R.y,R.z,1 });
		break;
	}

	vector_t Lo = vector_t(0.0);
	for (int i = 0; i < lights.size(); i++) {
		const Light* curLight = lights[i];

		vector_t L = vector_normalize(curLight->pos - v->pos_world);
		vector_t H = vector_normalize(V + L);

		float distance = vector_length(curLight->pos - v->pos_world);
		vector_t radiance = curLight->radiance / (distance * distance + 0.0001);

		//float roughness = 0.1;
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vector_t F = fresnelSchlick(max(vector_dot(H, V), 0.0f), F0);

		vector_t numerator = NDF * G * F;
		float denominator = 4.0 * max(vector_dot(N, V), 0.0f) * max(vector_dot(N, L), 0.0f) + 0.0001;
		vector_t specular = numerator / denominator;

		vector_t kS = F;
		vector_t kD = vector_t(1.0) - kS;
		kD = kD * (1.0 - metallic);

		float NdotL = max(vector_dot(N, L), 0.0f);
		Lo = Lo + (kD * albedo / PI + specular) * radiance * NdotL;
	}

	vector_t F = fresnelSchlickRoughness(max(vector_dot(N, V), 0.0f), F0, roughness);
	//vector_t F = fresnelSchlick(CMID(vector_dot(H, V), 0.0, 1.0), F0);

	vector_t kS = F;
	vector_t kD = 1.0 - kS;
	kD = kD * (1.0 - metallic);

	vector_t irradiance = cube_textures["irradiance"]->Read({ N.x,-N.y,N.z,1 });
	vector_t diffuse = irradiance * albedo;

	vector_t env_BRDF = textures["BRDF_LUT"]->Read(CMID(vector_dot(N, V), 0.0, 1.0), roughness);
	vector_t specular = prefilteredColor * (F * env_BRDF.x + env_BRDF.y);

	vector_t ambient = (kD * diffuse + specular) * ao;
	//vector_t ambient = vector_dot(vector_t(0.2), albedo) * ao;

	vector_t color = Lo + ambient;
	if (enable_hdr) {
		color = color / (color + vector_t(1.0));
	}

	if (enable_gamma)
	{
		color.x = pow(color.x, 1.0f / gamma);
		color.y = pow(color.y, 1.0f / gamma);
		color.z = pow(color.z, 1.0f / gamma);
	}

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