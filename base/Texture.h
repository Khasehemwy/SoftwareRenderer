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

class Texture_Cube{
public:
	int width = 1024;
	int height = 1024;
	std::array<Texture*, 6>textures;

	Texture_Cube(const std::array<std::filesystem::path, 6>& paths);
	color_t Read(const vector_t& dir);
};