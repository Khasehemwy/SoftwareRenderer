#include"Texture.h"

Texture::Texture()
{
	texture = create_2D_array<color_t>(width, height);
}
Texture::Texture(int width, int height):
	width(width),
	height(height)
{
	texture = create_2D_array<color_t>(width, height);
}
Texture::~Texture()
{
	free(texture);
}

void Texture::init()
{
	for (int j = 0; j < this->height; j++) {
		for (int i = 0; i < this->width; i++) {
			int x = i / 128, y = j / 128;
			this->texture[j][i] = ((x + y) & 1) ? color_trans_1f(0xffffff) : color_trans_1f(0x3fbcef);
		}
	}
}
