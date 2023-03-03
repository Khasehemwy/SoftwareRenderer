#pragma once
#include"Includes.h"

class Texture {
private:
	void Load(std::filesystem::path path);

public:
	bool isLoaded = false;

	int width = 1024;
	int height = 1024;
	color_t** texture;

	Texture(std::filesystem::path path);
	Texture(int width, int height);
	~Texture();

	void Set_Default_Tex();
	color_t Read(float u, float v, int choice = 1) const;
	void Write(float u, float v, const color_t& color);
};

class Texture_Cube{
public:
	int width = 1024;
	int height = 1024;
	std::array<Texture*, 6>textures;

	Texture_Cube(const std::array<std::filesystem::path, 6>& paths);
	color_t Read(const vector_t& dir);
	void Write(const vector_t& dir, const color_t& color);
};