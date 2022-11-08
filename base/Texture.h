#pragma once
#include"Includes.h"

class Texture {
private:
	void Load(std::filesystem::path path);

public:
	int width = 1024;
	int height = 1024;
	color_t** texture;

	Texture(std::filesystem::path path);
	Texture(int width, int height);
	~Texture();

	void Set_Default_Tex();
	color_t Read(float u, float v, int choice = 1) const;
};