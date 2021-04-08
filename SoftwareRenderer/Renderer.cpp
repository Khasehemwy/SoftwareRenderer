#include "Renderer.h"

Renderer::Renderer()
{
	features[RENDER_FEATURE_BACK_CULLING] = true;
}

void Renderer::init(int width, int height, void* fb)
{
	__int32 need = sizeof(void*) * (height * 2 + 1024) + (width * height * 8);
	char* ptr = (char*)malloc(need + 64);
	assert(ptr);
	char* framebuf, * zbuf;

	tex_limit_size = 1024;

	this->frame_buffer = (UINT**)ptr;
	this->z_buffer = (float**)(ptr + sizeof(void*) * height);
	ptr += sizeof(void*) * height * 2;
	this->texture = (UINT32**)ptr;
	ptr += sizeof(void*) * tex_limit_size;
	framebuf = (char*)ptr;
	zbuf = (char*)ptr + width * height * 4;
	ptr += width * height * 8;
	if (fb != NULL)framebuf = (char*)fb;
	for (int j = 0; j < height; j++) {
		this->frame_buffer[j] = (UINT32*)(framebuf + width * 4 * j);
		this->z_buffer[j] = (float*)(zbuf + width * 4 * j);
	}

	this->width = width;
	this->height = height;
	this->background = 0x1D4E89;
	this->foreground = 0x0;

	this->texture[0] = (UINT32*)ptr;
	this->texture[1] = (UINT32*)(ptr + 16);
	memset(this->texture[0], 0, 64);
	this->tex_width = 2;
	this->tex_height = 2;
	this->tex_max_u = 1.0f;
	this->tex_max_v = 1.0f;

	transform_init(&this->transform, width, height);
	this->render_state = RENDER_STATE_WIREFRAME;
}

void Renderer::destroy()
{
	if (this->frame_buffer)
		free(this->frame_buffer);
	this->frame_buffer = nullptr;
	this->z_buffer = nullptr;
	this->texture = nullptr;
}

void Renderer::clear()
{
	int height = this->height;
	for (int y = 0; y < this->height; y++) {
		for (int x = 0; x < this->width; x++) {
			this->frame_buffer[y][x] = this->background;
		}
	}
	for (int y = 0; y < this->height; y++) {
		for (int x = 0; x < this->width; x++) {
			this->z_buffer[y][x] = 0.0f;
		}
	}
}

void Renderer::set_texture(void* bits, long pitch, int w, int h)
{
	char* ptr = (char*)bits;
	assert(w <= tex_limit_size && h <= tex_limit_size);
	for (int j = 0; j < h; ptr += pitch, j++) 	// 重新计算每行纹理的指针
		this->texture[j] = (UINT32*)ptr;
	this->tex_width = w;
	this->tex_height = h;
	this->tex_max_u = (float)(w - 1);
	this->tex_max_v = (float)(h - 1);
}

UINT32 Renderer::texture_read(float u, float v)
{
	u = u * this->tex_max_u;
	v = v * this->tex_max_v;
	int x = (int)(u + 0.5f);
	int y = (int)(v + 0.5f);
	x = CMID(x, 0, this->tex_width - 1);
	y = CMID(y, 0, this->tex_height - 1);
	return this->texture[y][x];
}

void Renderer::draw_pixel(int x, int y, UINT32 color)
{
	if (((UINT32)x) < (UINT32)this->width && ((UINT32)y) < (UINT32)this->height) {
		this->frame_buffer[y][x] = color;
	}
}

//Bresenham算法
void Renderer::draw_line(int x1, int y1, int x2, int y2, UINT32 color)
{
	int dx = std::abs(x1 - x2);
	int dy = std::abs(y1 - y2);
	int rem = 0;//令e=d-0.5,每次x增加时y增加dyx(k),然后再令rem=(2e(dx)+(dx))/2,那么rem的范围就是[0,dx],每次增加dy.

	if (y2 < y1) { std::swap(y1, y2); std::swap(x1, x2); }
	int x = x1, y = y1;

	//直线均从上往下绘制
	//for (; y <= y2 && x >= min(x1, x2) && x <= max(x1, x2);) {
	//	if (dx > dy) {
	//		rem += dy;
	//		if (rem >= dx) {
	//			rem -= dx;
	//			y++;
	//		}
	//	}
	//	else {
	//		rem += dx;
	//		if (rem >= dy) {
	//			rem -= dy;
	//			x += (x2 > x1) ? 1 : -1;
	//		}
	//	}
	//	this->draw_pixel(x, y, color);
	//	if (dx > dy) { x += (x2 > x1) ? 1 : -1; }
	//	else { y++; }
	//}
	//和下方结果相同,比较次数更多,效率低一些
	 
	//直线可能从上往下,可能从左往右
	if (dx > dy) {
		if (x2 < x1) { std::swap(x1, x2); std::swap(y1, y2); }
		for (int x = x1, y = y1; x <= x2; x++) {
			rem += dy;
			if (rem >= dx) {
				rem -= dx;
				y += (y2 > y1) ? 1 : -1;
			}
			this->draw_pixel(x, y, color);
		}
	}
	else {
		if (y2 < y1) { std::swap(y1, y2); std::swap(x1, x2); }
		for (int x = x1, y = y1; y <= y2; y++) {
			rem += dx;
			if (rem >= dy) {
				rem -= dy;
				x += (x2 > x1) ? 1 : -1;
			}
			this->draw_pixel(x, y, color);
		}
	}

	this->draw_pixel(x1, y1, color);
	this->draw_pixel(x2, y2, color);
}

//拆分一般三角为特殊三角
void Renderer::draw_triangle(vertex_t v1, vertex_t v2, vertex_t v3)
{
	if (v1.pos.y > v2.pos.y) { std::swap(v1, v2); }
	if (v1.pos.y > v3.pos.y) { std::swap(v1, v3); }
	if (v2.pos.y > v3.pos.y) { std::swap(v2, v3); }
	//v1,v2,v3依次从上到下
	float x1, x2, x3, y1, y2, y3;
	x1 = v1.pos.x; y1 = v1.pos.y;
	x2 = v2.pos.x; y2 = v2.pos.y;
	x3 = v3.pos.x; y3 = v3.pos.y;

	if (y1 == y2 && y2 == y3)return;
	if (y1 == y2) {
		//平顶三角
		if (x1 > x2)std::swap(v1, v2);
		draw_triangle_StandardAlgorithm(v3, v1, v2);
		return;
	}
	if (y2 == y3) {
		//平底三角
		if (x2 > x3)std::swap(v2, v3);
		draw_triangle_StandardAlgorithm(v1, v2, v3);
		return;
	}

	//拆分三角
	vertex_t v4 = v2;
	float dxy = (x3 - x1) / (y3 - y1);
	v4.pos.x = (y2 - y1) * dxy + x1;
	float x4 = v4.pos.x, y4 = y2;
	//插值计算v4的相关数值
	float t = (y4 - y1) / (y3 - y1);
	v4.color = v1.color + (v3.color - v1.color) * t;
	v4.tex.u = v1.tex.u + (v3.tex.u - v1.tex.u) * t;
	v4.tex.v = v1.tex.v + (v3.tex.v - v1.tex.v) * t;
	v4.rhw = v1.rhw + (v3.rhw - v1.rhw) * t;

	if (x2 <= x4) {
		draw_triangle_StandardAlgorithm(v1, v2, v4);
		draw_triangle_StandardAlgorithm(v3, v2, v4);
	}
	else {
		draw_triangle_StandardAlgorithm(v1, v4, v2);
		draw_triangle_StandardAlgorithm(v3, v4, v2);
	}
}

void Renderer::draw_triangle_StandardAlgorithm(const vertex_t& top, const vertex_t& left, const vertex_t& right)
{
	//基于y的梯度,y增加1时,对应x/u/v/i增加的值
	float dxdy_l = (top.pos.x - left.pos.x) / (top.pos.y - left.pos.y);
	float dudy_l = (top.tex.u - left.tex.u) / (top.pos.y - left.pos.y);
	float dvdy_l = (top.tex.v - left.tex.v) / (top.pos.y - left.pos.y);
	float drhwdy_l = (top.rhw - left.rhw) / (top.pos.y - left.pos.y);
	float dxdy_r = (top.pos.x - right.pos.x) / (top.pos.y - right.pos.y);
	float dudy_r = (top.tex.u - right.tex.u) / (top.pos.y - right.pos.y);
	float dvdy_r = (top.tex.v - right.tex.v) / (top.pos.y - right.pos.y);
	float drhwdy_r = (top.rhw - right.rhw) / (top.pos.y - left.pos.y);
	float xl, ul, vl, rhwl;
	float xr, ur, vr, rhwr;

	//颜色插值
	color_t didy_l = (top.color - left.color) / (top.pos.y - left.pos.y);
	color_t didy_r = (top.color - right.color) / (top.pos.y - right.pos.y);
	color_t color_left, color_right;

	int y0 = (int)(ceil(top.pos.y));
	int y1 = (int)(ceil(left.pos.y));
	//绘制平底或平顶三角形
	if (y0 <= y1) {
		/*平底三角形*/
		//x初始值,并修正(浮点数转换整数需要修正)
		xl = top.pos.x + (ceil(top.pos.y) - top.pos.y) * dxdy_l;
		xr = top.pos.x + (ceil(top.pos.y) - top.pos.y) * dxdy_r;
		//纹理
		ul = top.tex.u + (ceil(top.pos.y) - top.pos.y) * dudy_l;
		vl = top.tex.v + (ceil(top.pos.y) - top.pos.y) * dvdy_l;
		ur = top.tex.u + (ceil(top.pos.y) - top.pos.y) * dudy_r;
		vr = top.tex.v + (ceil(top.pos.y) - top.pos.y) * dvdy_r;
		//颜色
		color_left = top.color + (ceil(top.pos.y) - top.pos.y) * didy_l;
		color_right = top.color + (ceil(top.pos.y) - top.pos.y) * didy_r;
		//深度
		rhwl = top.rhw + (ceil(top.pos.y) - top.pos.y) * drhwdy_l;
		rhwr = top.rhw + (ceil(top.pos.y) - top.pos.y) * drhwdy_r;
	}
	else {
		/*平顶三角形,类似平底三角形*/
		std::swap(y0, y1);
		xl = left.pos.x + (ceil(left.pos.y) - left.pos.y) * dxdy_l;
		xr = right.pos.x + (ceil(right.pos.y) - right.pos.y) * dxdy_r;

		ul = left.tex.u + (ceil(left.pos.y) - left.pos.y) * dudy_l;
		vl = left.tex.v + (ceil(left.pos.y) - left.pos.y) * dvdy_l;
		ur = right.tex.u + (ceil(right.pos.y) - right.pos.y) * dudy_r;
		vr = right.tex.v + (ceil(right.pos.y) - right.pos.y) * dvdy_r;

		color_left = left.color + (ceil(left.pos.y) - left.pos.y) * didy_l;
		color_right = right.color + (ceil(right.pos.y) - right.pos.y) * didy_r;

		rhwl = left.rhw + (ceil(left.pos.y) - left.pos.y) * drhwdy_l;
		rhwr = right.rhw + (ceil(right.pos.y) - right.pos.y) * drhwdy_r;
	}

	//从上往下绘制
	for (int y = y0; y < y1; y++) {
		float dx = xr - xl;
		float dux = (ur - ul) / dx;
		float dvx = (vr - vl) / dx;
		float drhwdx = (rhwr - rhwl) / dx;
		float ui = ul;
		float vi = vl;
		float rhwi = rhwl;
		color_t dix = (color_right - color_left) / dx;
		color_t color = color_left + (ceil(xl) - xl) * dix;

		for (int x = ceil(xl); x <= ceil(xr); x++) {
			if (x >= 0 && x < this->width && y >= 0 && y < this->height) {
				if (rhwi >= this->z_buffer[y][x]) {
					this->z_buffer[y][x] = rhwi;// 深度缓存
					float wi = 1.0 / rhwi;
					if (this->render_state == RENDER_STATE_COLOR) {
						this->draw_pixel(x, y, color_trans_255(color * wi));
					}
					if (this->render_state == RENDER_STATE_TEXTURE) {
						this->draw_pixel(x, y, this->texture_read(ui * wi, vi * wi));
					}
				}
			}
			color = color + dix;
			ui += dux;
			vi += dvx;
			rhwi += drhwdx;
		}

		xl += dxdy_l;
		ul += dudy_l;
		vl += dvdy_l;
		ur += dudy_r;
		vr += dvdy_r;
		xr += dxdy_r;
		color_left = color_left + didy_l;
		color_right = color_right + didy_r;
		rhwl += drhwdy_l;
		rhwr += drhwdy_r;
	}
}

void Renderer::add_light(const Light& light)
{
	this->lights.push_back(&light);
}

void Renderer::draw_triangle_BresenhamAlgorithm(const vertex_t& top, const vertex_t& left, const vertex_t& right)
{
	//FIX ME
	//没有完善颜色插值
	int x1_s = top.pos.x, y1_s = top.pos.y;
	int x1_e = top.pos.x, y1_e = top.pos.y;
	int x2 = left.pos.x, y2 = left.pos.y;
	int x3 = right.pos.x, y3 = right.pos.y;

	color_t color1_s = top.color;
	color_t color1_e = top.color;
	color_t color2 = left.color;
	color_t color3 = right.color;

	int dx_s = std::abs(x1_s - x2), dx_e = std::abs(x1_e - x3);
	int dy = std::abs(y1_s - y2);
	int y_s = min(y1_s, y2);
	int y_e = y_s;
	int rem_s = 0, rem_e = 0;
	int x_s, x_e;

	this->draw_pixel(x1_s, y1_s, 0x0);
	this->draw_pixel(x2, y2, 0x0);
	this->draw_pixel(x3, y3, 0x0);

	//left edge
	if (y2 < y1_s) { std::swap(y1_s, y2); std::swap(x1_s, x2); std::swap(color1_s, color2); }
	x_s = x1_s; y_s = y1_s;
	//right edge
	if (y3 < y1_e) { std::swap(y1_e, y3); std::swap(x1_e, x3); std::swap(color1_e, color3); }
	x_e = x1_e; y_e = y1_e;

	int jug_draw_scanline = true;

	while ((y_s <= y2 && x_s >= min(x1_s, x2) && x_s <= max(x1_s, x2))
		|| (y_e <= y2 && x_e >= min(x1_e, x3) && x_e <= max(x1_e, x3)))
	{
		//用于线性插值
		float s1, s2, p1, p2;
		if (dx_s > dy) { s1 = max(x1_s, x2) - min(x1_s, x2); p1 = x_s; }
		else { s1 = y2 - y1_s; p1 = y_s; }
		if (dx_e > dy) { s2 = max(x1_e, x3) - min(x1_e, x3); p2 = x_e; }
		else { s2 = y3 - y1_e; p2 = y_e; }

		if (y_s == y_e) {//相同y值的扫描线只画一次
			if (jug_draw_scanline == true) {
				//FIX ME
				color_t color_left, color_right;
				int x = x_s, y = y_s;
				color_left.r = interp(s1, p1, color1_s.r, color2.r);
				color_left.g = interp(s1, p1, color1_s.g, color2.g);
				color_left.b = interp(s1, p1, color1_s.b, color2.b);
				color_left.a = interp(s1, p1, color1_s.a, color2.a);
				color_right.r = interp(s1, p2, color1_e.r, color2.r);
				color_right.g = interp(s1, p2, color1_e.g, color2.g);
				color_right.b = interp(s1, p2, color1_e.b, color2.b);
				color_right.a = interp(s1, p2, color1_e.a, color2.a);
				float length_x = x_e - x_s;
				for (; x <= x_e; x++) {
					color_t color;
					color.r = interp(length_x, x - x_s, color_left.r, color_right.r);
					color.g = interp(length_x, x - x_s, color_left.g, color_right.g);
					color.b = interp(length_x, x - x_s, color_left.b, color_right.b);
					color.a = interp(length_x, x - x_s, color_left.a, color_right.a);
					this->draw_pixel(x, y, color_trans_255(color));
				}
				//this->draw_line(x_s, y_s, x_e, y_e, 0x0);
				jug_draw_scanline = false;
			}
		}
		else { jug_draw_scanline = true; }

		//left
		if (y_s <= y_e) {
			if (dx_s > dy) {
				rem_s += dy;
				if (rem_s >= dx_s) {
					rem_s -= dx_s;
					y_s++;
				}
			}
			else {
				rem_s += dx_s;
				if (rem_s >= dy) {
					rem_s -= dy;
					x_s += (x2 > x1_s) ? 1 : -1;
				}
			}
			this->draw_pixel(x_s, y_s, 0x0);
			if (dx_s > dy) { x_s += (x2 > x1_s) ? 1 : -1; }
			else { y_s++; }
		}

		if (y_s == y_e) {//相同y值的扫描线只画一次
			if (jug_draw_scanline == true) {
				//FIX ME
				color_t color_left, color_right;
				int x = x_s, y = y_s;
				color_left.r = interp(s1, p1, color1_s.r, color2.r);
				color_left.g = interp(s1, p1, color1_s.g, color2.g);
				color_left.b = interp(s1, p1, color1_s.b, color2.b);
				color_left.a = interp(s1, p1, color1_s.a, color2.a);
				color_right.r = interp(s1, p2, color1_e.r, color2.r);
				color_right.g = interp(s1, p2, color1_e.g, color2.g);
				color_right.b = interp(s1, p2, color1_e.b, color2.b);
				color_right.a = interp(s1, p2, color1_e.a, color2.a);
				float length_x = x_e - x_s;
				for (; x <= x_e; x++) {
					color_t color;
					color.r = interp(length_x, x - x_s, color_left.r, color_right.r);
					color.g = interp(length_x, x - x_s, color_left.g, color_right.g);
					color.b = interp(length_x, x - x_s, color_left.b, color_right.b);
					color.a = interp(length_x, x - x_s, color_left.a, color_right.a);
					this->draw_pixel(x, y, color_trans_255(color));
				}
				//this->draw_line(x_s, y_s, x_e, y_e, 0x0);
				jug_draw_scanline = false;
			}
		}
		else { jug_draw_scanline = true; }

		//right
		if (y_e <= y_s) {
			if (dx_e > dy) {
				rem_e += dy;
				if (rem_e >= dx_e) {
					rem_e -= dx_e;
					y_e++;
				}
			}
			else {
				rem_e += dx_e;
				if (rem_e >= dy) {
					rem_e -= dy;
					x_e += (x3 > x1_e) ? 1 : -1;
				}
			}
			this->draw_pixel(x_e, y_e, 0x0);
			if (dx_e > dy) { x_e += (x3 > x1_e) ? 1 : -1; }
			else { y_e++; }
		}
	}
}

//判断点是否在三角形内
float sign(const point_t& p1, const point_t& p2, const point_t& p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}
bool PointInTriangle(const point_t &pt, const point_t& v1, const point_t& v2, const point_t& v3)
{
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(pt, v1, v2);
	d2 = sign(pt, v2, v3);
	d3 = sign(pt, v3, v1);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}
void Renderer::draw_triangle_BoundingBox(const vertex_t& v1, const vertex_t& v2, const vertex_t& v3)
{
	//TODO
	//没有颜色插值
	float r, g, b, a;
	r = v1.color.r;
	g = v1.color.g;
	b = v1.color.b;
	int R = (int)(r * 255.0f);
	int G = (int)(g * 255.0f);
	int B = (int)(b * 255.0f);
	R = CMID(R, 0, 255);
	G = CMID(G, 0, 255);
	B = CMID(B, 0, 255);
	UINT color = (R << 16) | (G << 8) | (B);

	int maxX = max(v1.pos.x, max(v2.pos.x, v3.pos.x));
	int minX = min(v1.pos.x, min(v2.pos.x, v3.pos.x));
	int maxY = max(v1.pos.y, max(v2.pos.y, v3.pos.y));
	int minY = min(v1.pos.y, min(v2.pos.y, v3.pos.y));

	vector_t vec1 = v2.pos - v1.pos;
	vector_t vec2 = v3.pos - v1.pos;

	for (int y = minY; y <= maxY; y++) {
		for (int x = minX; x <= maxX; x++) {
			point_t p; p.x = x; p.y = y;
			if(PointInTriangle(p,v1.pos,v2.pos,v3.pos))
			{ /* inside triangle */
				this->draw_pixel(x, y, color);
			}
		}
	}
}

//绘制原始三角形
int Renderer::display_primitive(const vertex_t& v1, const vertex_t& v2, const vertex_t& v3)
{
	point_t p1, p2, p3;

	/* 将点映射到世界空间 */
	p1 = (v1.pos) * this->transform.model;
	p2 = (v2.pos) * this->transform.model;
	p3 = (v3.pos) * this->transform.model;

	// 进行背面剔除(点的排列必须为右手螺旋后法向量朝外)
	vector_t p1_p2 = p2 - p1, p1_p3 = p3 - p1;
	vector_t v_normal = vector_cross(p1_p3, p1_p2);
	vector_t v_view = this->camera->camera_pos - p1;
	if (features[RENDER_FEATURE_BACK_CULLING]) {
		float backCull_jug = vector_dot(v_normal, v_view);
		if (backCull_jug <= 0.0f)return 1;
	}

	/* 将点映射到观察空间 */
	p1 = p1 * this->transform.view;
	p2 = p2 * this->transform.view;
	p3 = p3 * this->transform.view;

	vertex_t v1_tmp, v2_tmp, v3_tmp;
	v1_tmp = v1; v2_tmp = v2; v3_tmp = v3;
	for (auto& light : lights) {
		// 环境光
		color_t ambient1 = light->ambient * v1_tmp.color;
		color_t ambient2 = light->ambient * v2_tmp.color;
		color_t ambient3 = light->ambient * v3_tmp.color;
		// 漫反射
		// 使用兰伯特余弦定律（Lambert' cosine law）计算漫反射
		vector_t norm = vector_normalize(v_normal);
		vector_t light_dir;
		float diff;
		light_dir = vector_normalize(light->pos - p1);
		diff = max(vector_dot(norm, light_dir), 0.0f);
		color_t diffuse1 = light->diffuse * diff * v1_tmp.color;

		light_dir = vector_normalize(light->pos - p2);
		diff = max(vector_dot(norm, light_dir), 0.0f);
		color_t diffuse2 = light->diffuse * diff * v2_tmp.color;

		light_dir = vector_normalize(light->pos - p3);
		diff = max(vector_dot(norm, light_dir), 0.0f);
		color_t diffuse3 = light->diffuse * diff * v3_tmp.color;
		// 镜面反射
		vector_t reflect_dir = vector_reflect(-light_dir, norm);
		reflect_dir = vector_normalize(reflect_dir);

		vector_t view_dir = vector_normalize(camera->camera_pos - p1);
		float shininess = 32.0f;
		float spec = pow(max(vector_dot(view_dir, reflect_dir), 0.0), shininess);
		color_t specular1 = light->specular * spec * v1_tmp.color;

		view_dir = vector_normalize(camera->camera_pos - p2);
		shininess = 32.0f;
		spec = pow(max(vector_dot(view_dir, reflect_dir), 0.0), shininess);
		color_t specular2 = light->specular * spec * v2_tmp.color;

		view_dir = vector_normalize(camera->camera_pos - p3);
		shininess = 32.0f;
		spec = pow(max(vector_dot(view_dir, reflect_dir), 0.0), shininess);
		color_t specular3 = light->specular * spec * v3_tmp.color;

		//光照运算
		v1_tmp.color = (ambient1 + diffuse1 + specular1);
		v2_tmp.color = (ambient2 + diffuse2 + specular2);
		v3_tmp.color = (ambient3 + diffuse3 + specular3);
	}


	/* 将点映射到裁剪空间 */
	p1 = p1 * this->transform.projection;
	p2 = p2 * this->transform.projection;
	p3 = p3 * this->transform.projection;

	// 裁剪检测
	int cvv_jug = 0;
	if (check_cvv(p1) != 0 && check_cvv(p2) != 0 && check_cvv(p3) != 0)cvv_jug = 1;
	if (cvv_jug) {
		//std::cout << "cvv cut\n";
		return 1;
	}

	//得到屏幕坐标
	p1 = viewport_transform(p1, this->transform);
	p2 = viewport_transform(p2, this->transform);
	p3 = viewport_transform(p3, this->transform);


	// 线框绘制
	if (this->render_state == RENDER_STATE_WIREFRAME) {
		this->draw_line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, this->foreground);
		this->draw_line((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y, this->foreground);
		this->draw_line((int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, this->foreground);
	}
	else if(this->render_state) {
		v1_tmp.pos = p1; 
		v2_tmp.pos = p2;
		v3_tmp.pos = p3;
		vertex_set_rhw(&v1_tmp);
		vertex_set_rhw(&v2_tmp);
		vertex_set_rhw(&v3_tmp);
		if (render_shader_state == RENDER_SHADER_PIXEL_SCANLINE)//default
			this->draw_triangle(v1_tmp, v2_tmp, v3_tmp);
		else if (render_shader_state == RENDER_SHADER_PIXEL_BOUNDINGBOX)
			this->draw_triangle_BoundingBox(v1_tmp, v2_tmp, v3_tmp);
	}

	return 0;
}



void Renderer::transform_update()
{
	this->transform.transform = transform.model * transform.view * transform.projection;
}
