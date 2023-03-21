#include"Includes.h"

void Renderer::draw_triangle_EdgeEquation(const vertex_t& v1, const vertex_t& v2, const vertex_t& v3)
{
	int maxX = max(v1.pos.x, max(v2.pos.x, v3.pos.x));
	int minX = min(v1.pos.x, min(v2.pos.x, v3.pos.x));
	int maxY = max(v1.pos.y, max(v2.pos.y, v3.pos.y));
	int minY = min(v1.pos.y, min(v2.pos.y, v3.pos.y));

	for (int y = minY; y <= maxY; y++) {
		for (int x = minX; x <= maxX; x++) {
			//用重心插值获取其他属性
			barycentric_t bary = Get_Barycentric(
				{ (float)x , (float)y , 0 , 0 },
				v1.pos,
				v2.pos,
				v3.pos);

			if (bary.w1 >= 0 && bary.w2 >= 0 && bary.w3 >= 0) {
				Draw_Fragment(&v1, &v2, &v3, bary, x, y);
			}
		}
	}
}
