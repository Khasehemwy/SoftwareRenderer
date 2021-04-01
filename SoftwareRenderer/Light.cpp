#include"Light.h"

void Light::set_vertex()
{
	for (auto& v : *vertexs) {
		float dis = vector_length(v.pos - this->pos);
		//while (dis > 1)dis *= 0.1;
		v.color = v.color * dis;
	}
}
