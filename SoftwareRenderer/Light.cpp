#include"Light.h"

Light::Light():
	cut_off(cos(radians(12.5f))),
	outer_cut_off(cos(radians(17.5f)))
{
	//shadow_map = new Texture();
}

void Light::Init_ShadowMap(int width, int height)
{
	this->shadow_map = new Texture(width, height);
}

void Light::Set_ShadowMap(const Renderer& renderer)
{
	float dxdu = renderer.width / ((float)this->shadow_map->width);
	float dydv = renderer.height / ((float)this->shadow_map->height);
	float x = 0.0f, y = 0.0f;
	for (int v = 0; v < shadow_map->height; v++) {
		x = 0.0f;
		for (int u = 0; u < shadow_map->width; u++) {
			this->shadow_map->texture[v][u].r = 1 / renderer.z_buffer[(int)y][(int)x];
			x += dxdu;
		}
		y += dydv;
	}
	this->light_space_matrix = renderer.transform.view * renderer.transform.projection;
}
