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

void Light::Set_Shadow_Map(const Renderer& renderer)
{
	for (int j = 0; j < renderer.height; j++) {
		for (int i = 0; i < renderer.width; i++) {
			this->shadow_map->texture[j][i].r = 1 / renderer.z_buffer[j][i];
		}
	}
	this->light_space_matrix = renderer.transform.view * renderer.transform.projection;
}
