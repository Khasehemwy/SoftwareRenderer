#pragma once
#include"Includes.h"

class Texture {
public:
	int width = 1024;
	int height = 1024;
	color_t** texture;

	Texture();
	Texture(int width, int height);
	~Texture();

	void init();
	color_t Read(float u, float v, int choice = 1);
};