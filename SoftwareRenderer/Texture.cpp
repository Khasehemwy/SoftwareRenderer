#include"Texture.h"

void Texture::init()
{
	for (int j = 0; j < this->max_size; j++) {
		for (int i = 0; i < this->max_size; i++) {
			int x = i / 32, y = j / 32;
			this->texture[j][i] = ((x + y) & 1) ? 0xffffff : 0x3fbcef;
		}
	}
}
