﻿#include"Texture.h"

Texture::Texture(std::filesystem::path path)
{
	Load(path);
}

Texture::Texture(int width, int height):
	width(width),
	height(height)
{
	texture = create_2D_array<color_t>(width, height);
}

Texture::~Texture()
{
	delete[] texture;
}

void Texture::Set_Default_Tex()
{
	for (int j = 0; j < this->height; j++) {
		for (int i = 0; i < this->width; i++) {
			int x = i / 128, y = j / 128;
			this->texture[j][i] = ((x + y) & 1) ? color_trans_1f(0xffffff) : color_trans_1f(0x3fbcef);
		}
	}
}

void Texture::Load(std::filesystem::path path)
{
	int width, height, nrComponents;
	unsigned char* data = stbi_load(
		path.string().c_str(), 
		&width, &height, 
		&nrComponents, 0);

	if (data)
	{
		if (texture) {
			delete[] texture;
		}

		this->width = width;
		this->height = height;
		texture = create_2D_array<color_t>(width, height);

		if (nrComponents == 1) {
			// RED

		}
		else if (nrComponents == 3) {
			// RGB
			color_t color;
			int data_index = 0;
			for (int j = 0; j < height; j++) {
				for (int i = 0; i < width; i++) {
					color.r = (float)data[data_index++] / 255.0f;
					color.g = (float)data[data_index++] / 255.0f;
					color.b = (float)data[data_index++] / 255.0f;
					color.a = 1;
					this->texture[j][i] = color;
				}
			}
		}
		else if (nrComponents == 4) {
			// RGBA
			color_t color;
			int data_index = 0;
			for (int j = 0; j < height; j++) {
				for (int i = 0; i < width; i++) {
					color.r = (float)data[data_index++] / 255.0f;
					color.g = (float)data[data_index++] / 255.0f;
					color.b = (float)data[data_index++] / 255.0f;
					color.a = (float)data[data_index++] / 255.0f;
					this->texture[j][i] = color;
				}
			}
		}

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
}

color_t Texture::Read(float u, float v, int choice) const
{

	u = u * this->width;
	v = v * this->height;

	if (choice == 0) {
		//点采样
		int x = (int)(u + 0.5f);
		int y = (int)(v + 0.5f);
		x = CMID(x, 0, this->width - 1);
		y = CMID(y, 0, this->height - 1);
		return this->texture[y][x];
	}

	else if (choice == 1) {
		//双线性滤波
		int u_0 = CMID(floor(u), 0, this->width - 1);
		int u_1 = CMID(u_0 + 1, 0, this->width - 1);
		int v_0 = CMID(floor(v), 0, this->height - 1);
		int v_1 = CMID(v_0 + 1, 0, this->height - 1);
		float du_0 = u - floor(u);
		float du_1 = floor(u) + 1 - u;
		float dv_0 = v - floor(v);
		float dv_1 = floor(v) + 1 - v;
		color_t c_up, c_down, color;
		c_up = this->texture[v_0][u_0] * du_1 + this->texture[v_0][u_1] * du_0;
		c_down = this->texture[v_1][u_0] * du_1 + this->texture[v_1][u_1] * du_0;
		color = c_up * dv_1 + c_down * dv_0;
		return color;
	}
}
