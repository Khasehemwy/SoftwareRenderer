#include"Light.h"

Light::Light():
	cut_off(cos(radians(15.0f))),
	outer_cut_off(cos(radians(20.0f)))
{
}
