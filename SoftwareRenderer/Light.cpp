#include"Light.h"

Light::Light():
	cut_off(cos(radians(12.5f))),
	outer_cut_off(cos(radians(17.5f)))
{
	//shadow_map = new Texture();
}
