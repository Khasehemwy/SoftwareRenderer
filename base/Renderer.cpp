#include "Renderer.h"
#pragma warning (disable:4996);

Renderer::Renderer()
{
	features[RENDER_FEATURE_BACK_CULLING] = true;
	features[RENDER_FEATURE_LIGHT] = true;
	features[RENDER_FEATURE_CVV_CLIP] = true;
	features[RENDER_FEATURE_SHADOW] = true;
	features[RENDER_FEATURE_LIGHT_PHONG] = true;
	features[RENDER_FEATURE_AUTO_NORMAL] = true;
}

void Renderer::init(int width, int height, void* fb)
{
	//__int32 need = sizeof(UINT32) * (height * 2) + (width * height * 2);
	//char* ptr = (char*)malloc(need + 64);
	//assert(ptr);
	//char* framebuf, * zbuf;

	//this->frame_buffer = (UINT**)ptr;
	//this->z_buffer = (float**)(ptr + sizeof(void*) * height);
	//ptr += sizeof(void*) * height * 2;
	//this->texture = (color_t**)ptr;
	//framebuf = (char*)ptr;
	//zbuf = (char*)ptr + width * height * sizeof(void*);
	//if (fb != NULL)framebuf = (char*)fb;
	//for (int j = 0; j < height; j++) {
	//	this->frame_buffer[j] = (UINT32*)(framebuf + width * sizeof(void*) * j);
	//	this->z_buffer[j] = (float*)(zbuf + width * sizeof(void*) * j);
	//}
	__int32 need = sizeof(UINT32) * ((height)+(width * height));
	char* ptr = (char*)malloc(need + 64);
	assert(ptr);
	char* framebuf;
	this->frame_buffer = (UINT32**)ptr;
	ptr += sizeof(void*) * height;
	framebuf = (char*)ptr;
	if (fb != NULL)framebuf = (char*)fb;
	for (int j = 0; j < height; j++) {
		this->frame_buffer[j] = (UINT32*)(framebuf + width * sizeof(void*) * j);
	}

	z_buffer = create_2D_array<float>(height, width);

	this->width = width;
	this->height = height;
	this->background = 0x1D4E89;
	this->background_f = color_trans_1f(background);
	this->foreground = 0x0;

	tex_limit_size = 8192;
	this->texture = new Texture(width, height);

	this->tex_width = 2;
	this->tex_height = 2;
	this->tex_max_u = 1.0f;
	this->tex_max_v = 1.0f;

	this->min_clip_x = min_clip_y = 0;
	this->max_clip_x = width;
	this->max_clip_y = height;

	transform_init(&this->transform, width, height);
	this->render_state = RENDER_STATE_COLOR;
}

void Renderer::destroy()
{
	if (this->frame_buffer)
		free(this->frame_buffer);
	this->frame_buffer = nullptr;
	this->z_buffer = nullptr;
	this->texture->texture = nullptr;
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

void Renderer::set_texture(const Texture& tex)
{
	texture->texture = tex.texture;
	int w = tex.width;
	int h = tex.height;
	assert(w <= tex_limit_size && h <= tex_limit_size);
	for (int j = 0; j < h; j++) 	// 重新计算每行纹理的指针
		this->texture->texture[j] = (color_t*)(*texture->texture + w * j);
	this->tex_width = w;
	this->tex_height = h;
	this->tex_max_u = (float)(w - 1);
	this->tex_max_v = (float)(h - 1);
}

color_t Renderer::texture_read(const Texture& tex, float u, float v)
{
	u = u * this->tex_max_u;
	v = v * this->tex_max_v;

	//点采样
	//int x = (int)(u + 0.5f);
	//int y = (int)(v + 0.5f);
	//x = CMID(x, 0, this->tex_width - 1);
	//y = CMID(y, 0, this->tex_height - 1);
	//return tex.texture[y][x];

	//双线性滤波
	int u_0 = CMID(floor(u), 0, this->tex_width - 1);
	int u_1 = CMID(u_0 + 1, 0, this->tex_width - 1);
	int v_0 = CMID(floor(v), 0, this->tex_height - 1);
	int v_1 = CMID(v_0 + 1, 0, this->tex_height - 1);
	float du_0 = u - floor(u);
	float du_1 = floor(u) + 1 - u;
	float dv_0 = v - floor(v);
	float dv_1 = floor(v) + 1 - v;
	color_t c_up, c_down, color;
	c_up = tex.texture[v_0][u_0] * du_1 + tex.texture[v_0][u_1] * du_0;
	c_down = tex.texture[v_1][u_0] * du_1 + tex.texture[v_1][u_1] * du_0;
	color = c_up * dv_1 + c_down * dv_0;
	return color;
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
void Renderer::draw_triangle(vertex_t v1, vertex_t v2, vertex_t v3, Draw_ExtraData extra_data)
{
	if (v1.pos.y > v2.pos.y) { 
		std::swap(v1, v2); 
		if (extra_data.world_pos.valid) {
			std::swap(extra_data.world_pos.p1, extra_data.world_pos.p2);
		}
	}
	if (v1.pos.y > v3.pos.y) { 
		std::swap(v1, v3); 
		if (extra_data.world_pos.valid) {
			std::swap(extra_data.world_pos.p1, extra_data.world_pos.p3);
		}
	}
	if (v2.pos.y > v3.pos.y) { 
		std::swap(v2, v3); 
		if (extra_data.world_pos.valid) {
			std::swap(extra_data.world_pos.p2, extra_data.world_pos.p3);
		}
	}

	//v1,v2,v3依次从上到下
	float x1, x2, x3, y1, y2, y3;
	x1 = v1.pos.x; y1 = v1.pos.y;
	x2 = v2.pos.x; y2 = v2.pos.y;
	x3 = v3.pos.x; y3 = v3.pos.y;

	if (y1 == y2 && y2 == y3)return;
	if (y1 == y2) {
		//平顶三角
		if (x1 > x2) {
			std::swap(v1, v2);
			if (extra_data.world_pos.valid) {
				std::swap(extra_data.world_pos.p1, extra_data.world_pos.p2);
			}
		}

		if (extra_data.world_pos.valid) {
			std::swap(extra_data.world_pos.p1, extra_data.world_pos.p3);
			std::swap(extra_data.world_pos.p2, extra_data.world_pos.p3);
		}
		if (extra_data.valid) {
			draw_triangle_StandardAlgorithm(v3, v1, v2, extra_data);
		}
		else {
			draw_triangle_StandardAlgorithm(v3, v1, v2, extra_data);
		}
		return;
	}
	if (y2 == y3) {
		//平底三角
		if (x2 > x3) {
			std::swap(v2, v3);
			if (extra_data.world_pos.valid) {
				std::swap(extra_data.world_pos.p2, extra_data.world_pos.p3);
			}
		}
		if (extra_data.valid) {
			draw_triangle_StandardAlgorithm(v1, v2, v3, extra_data);
		}
		else {
			draw_triangle_StandardAlgorithm(v1, v2, v3, extra_data);
		}
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
	//插值计算v4的z值,用于阴影
	point_t world_p4;
	if (extra_data.world_pos.valid) {
		const point_t& world_p1 = extra_data.world_pos.p1;
		const point_t& world_p2 = extra_data.world_pos.p2;
		const point_t& world_p3 = extra_data.world_pos.p3;
		world_p4 = extra_data.world_pos.p2;
		world_p4 = world_p1 + (world_p3 - world_p1) * t;
	}

	if (x2 <= x4) {
		if (extra_data.world_pos.valid) {
			Draw_ExtraData extra_data_tmp;
			extra_data_tmp = extra_data;

			extra_data_tmp.world_pos.p1 = extra_data.world_pos.p1;
			extra_data_tmp.world_pos.p2 = extra_data.world_pos.p2;
			extra_data_tmp.world_pos.p3 = world_p4;
			draw_triangle_StandardAlgorithm(v1, v2, v4, extra_data_tmp);

			extra_data_tmp.world_pos.p1 = extra_data.world_pos.p3;
			extra_data_tmp.world_pos.p2 = extra_data.world_pos.p2;
			extra_data_tmp.world_pos.p3 = world_p4;
			draw_triangle_StandardAlgorithm(v3, v2, v4, extra_data_tmp);
		}
		else {
			draw_triangle_StandardAlgorithm(v1, v2, v4, extra_data);
			draw_triangle_StandardAlgorithm(v3, v2, v4, extra_data);
		}
	}
	else {
		if (extra_data.world_pos.valid) {
			Draw_ExtraData extra_data_tmp;
			extra_data_tmp = extra_data;

			extra_data_tmp.world_pos.p1 = extra_data.world_pos.p1;
			extra_data_tmp.world_pos.p2 = world_p4;
			extra_data_tmp.world_pos.p3 = extra_data.world_pos.p2;
			draw_triangle_StandardAlgorithm(v1, v4, v2, extra_data_tmp);

			extra_data_tmp.world_pos.p1 = extra_data.world_pos.p3;
			extra_data_tmp.world_pos.p2 = world_p4;
			extra_data_tmp.world_pos.p3 = extra_data.world_pos.p2;
			draw_triangle_StandardAlgorithm(v3, v4, v2, extra_data_tmp);
		}
		else {
			draw_triangle_StandardAlgorithm(v1, v4, v2, extra_data);
			draw_triangle_StandardAlgorithm(v3, v4, v2, extra_data);
		}
	}
}

void Renderer::draw_triangle_StandardAlgorithm(const vertex_t& top, const vertex_t& left, const vertex_t& right, const Draw_ExtraData& extra_data)
{
	//注意所有需要插值的数据都需要透视修正,即所有值都乘上了rhw

	//基于y的梯度,y增加1时,对应x/u/v/i/rhw增加的值
	float dxdy_l = (top.pos.x - left.pos.x) / (top.pos.y - left.pos.y);
	float dxdy_r = (top.pos.x - right.pos.x) / (top.pos.y - right.pos.y);
	
	float xl;
	float xr;

	int y0 = 0, y1 = 0;
	float y0f = top.pos.y, y1f = left.pos.y;
	//赋初始值,暂不修正
	//平底或平顶三角形
	if (y0f <= y1f) {
		/*平底三角形*/
		xl = top.pos.x;
		xr = top.pos.x;
	}
	else {
		/*平顶三角形,类似平底三角形*/
		std::swap(y0, y1);
		std::swap(y0f, y1f);
		xl = left.pos.x;
		xr = right.pos.x;
	}

	//垂直裁剪
	if (y1f < min_clip_y) { return; }
	if (y0f < min_clip_y) {
		float dy = min_clip_y - y0f;
		xl += dxdy_l * dy;
		xr += dxdy_r * dy;
		y0f = min_clip_y;
	}
	y0 = (int)(ceil(y0f));
	y1 = (int)(ceil(y1f));
	//修正(浮点数转换整数需要修正)
	float delta = y0 - y0f;
	xl += delta * dxdy_l;
	xr += delta * dxdy_r;

	vertex_t v1 = top;
	vertex_t v2 = left;
	vertex_t v3 = right;

	v1.pos_world = extra_data.world_pos.p1;
	v2.pos_world = extra_data.world_pos.p2;
	v3.pos_world = extra_data.world_pos.p3;

	//从上往下绘制
	for (int y = y0; y < y1; y++) {
		if (y >= max_clip_y)break;
		float dx = xr - xl;

		float xli = xl, xri = xr;
		//调整到下一步
		xl += dxdy_l;
		xr += dxdy_r;

		//水平裁剪
		if (xri < min_clip_x) { continue; }
		if (xli < min_clip_x) {
			float dx = min_clip_x - xli;

			xli = min_clip_x;
		}

		//修正
		int x0 = (int)ceil(xli);
		int x1 = (int)ceil(xri);
		float delta = x0 - xli;

		for (int x = x0; x <= x1; x++) {
			if (x >= max_clip_x)break;
			if (x >= 0 && y >= 0) {

				//用重心插值获取其他属性
				barycentric_t bary = Get_Barycentric(
					{ (float)x , (float)y , 0 , 0 },
					v1.pos, 
					v2.pos, 
					v3.pos);

				float rhwi = v1.rhw * bary.w1 + v2.rhw * bary.w2 + v3.rhw * bary.w3;

				if (rhwi >= this->z_buffer[y][x]) {

					this->z_buffer[y][x] = rhwi;// 深度缓存

					this->draw_pixel(
						x, y, 
						color_trans_255(PS_Interpolation(&v1, &v2, &v3, bary)));
				}
			}
		}
	}
}


void Renderer::add_light(const Light& light)
{
	this->lights.push_back(&light);
}

int Renderer::Set_Feature(UINT32 feature, bool turn_on)
{
	this->features[feature] = turn_on;

	return 1;
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

	//计算重心坐标系数,并用系数判断是否在三角形内 (和上方应该是一样的原理)
	//barycentric_t w = Get_Barycentric(pt, v1, v2, v3);
	//if (w.w1 > 0 && w.w1 < 1 && w.w2>0 && w.w2 < 1 && w.w3>0 && w.w3 < 1) { return true; }
	//else return false;
}
void Renderer::draw_triangle_BoundingBox(const vertex_t& v1, const vertex_t& v2, const vertex_t& v3)
{
	//TODO
	//没有颜色插值
	float r, g, b;
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

void Renderer::Phong_Shading(vertex_t& v, bool in_shadow)
{
	//光照处理
	if (this->render_state == RENDER_STATE_TEXTURE) {
		v.color = { 1,1,1,1 };
	}
	color_t color;
	color = { 0,0,0,0 };
	for (auto& light : lights) {
		color += Calculate_Lighting(v, light, in_shadow);
	}

	v.color = color;
}

color_t Renderer::Calculate_Lighting(vertex_t& v, const Light* light, bool in_shadow)
{
	color_t color = { 0,0,0,0 };

	// 环境光
	color_t ambient = light->ambient * v.color;

	// 漫反射
	// 使用兰伯特余弦定律（Lambert' cosine law）计算漫反射
	vector_t norm = vector_normalize(v.normal);
	vector_t light_dir;
	float diff;
	if (light->light_state == LIGHT_STATE_DIRECTIONAL) {
		light_dir = vector_normalize(-light->direction);
	}
	else if (
		light->light_state == LIGHT_STATE_POINT ||
		light->light_state == LIGHT_STATE_SPOTLIGHT)
	{
		light_dir = vector_normalize(light->pos - v.pos);
	}

	diff = max(vector_dot(norm, light_dir), 0.0f);
	color_t diffuse = light->diffuse * diff * v.color;

	// 镜面反射
	vector_t reflect_dir;
	vector_t view_dir;
	reflect_dir = vector_reflect(-light_dir, norm);
	view_dir = vector_normalize(camera->pos - v.pos);
	float shininess = 32.0f;

	float spec = pow(max(vector_dot(view_dir, reflect_dir), 0.0f), shininess);
	color_t specular = light->specular * spec * v.color;

	//光照运算
	if (light->light_state == LIGHT_STATE_DIRECTIONAL) {
		if (in_shadow && features[RENDER_FEATURE_LIGHT_PHONG]) {
			color += ambient;
		}
		else {
			color += (ambient + diffuse + specular);
		}
	}
	else if (light->light_state == LIGHT_STATE_POINT ||
		light->light_state == LIGHT_STATE_SPOTLIGHT)
	{
		//衰减
		float distance = vector_length(light->pos - v.pos);
		float attenuation = 1.0f / (light->constant + light->linear * distance +
			light->quadratic * (distance * distance));

		//聚光
		if (light->light_state == LIGHT_STATE_SPOTLIGHT) {
			float epsilon = light->cut_off - light->outer_cut_off;
			float theta;
			float intensity;
			vector_t light_direction = vector_normalize(-light->direction);

			theta = vector_dot(light_dir, light_direction);
			intensity = CMID((theta - light->outer_cut_off) / epsilon, 0.0, 1.0);
			ambient *= intensity; diffuse *= intensity; specular *= intensity;
		}

		if (in_shadow && features[RENDER_FEATURE_LIGHT_PHONG]) {
			color += ambient * attenuation;
		}
		else {
			color += (ambient + diffuse + specular) * attenuation;
		}
	}
	else {
		if (in_shadow && features[RENDER_FEATURE_LIGHT_PHONG]) {
			color += ambient;
		}
		else {
			color += (ambient + diffuse + specular);
		}
	}
	return color;
}

//绘制原始三角形
int Renderer::display_primitive(vertex_t v1, vertex_t v2, vertex_t v3)
{
	point_t p1, p2, p3;
	Draw_ExtraData extra_data;

	// 先裁剪检测,减小不必要计算
	if (features[RENDER_FEATURE_CVV_CLIP]) {
		p1 = v1.pos * transform.model * transform.view * transform.projection;
		p2 = v2.pos * transform.model * transform.view * transform.projection;
		p3 = v3.pos * transform.model * transform.view * transform.projection;
		int cvv_jug = 0;
		if (check_cvv(p1) != 0 && check_cvv(p2) != 0 && check_cvv(p3) != 0)cvv_jug = 1;
		if (cvv_jug) {
			//std::cout << "cvv cut\n";
			return 1;
		}
	}

	/* 将点映射到世界空间 */
	p1 = (v1.pos) * this->transform.model;
	p2 = (v2.pos) * this->transform.model;
	p3 = (v3.pos) * this->transform.model;

	if (features[RENDER_FEATURE_SHADOW] || features[RENDER_FEATURE_LIGHT_PHONG]) {
		extra_data.valid = true;
		extra_data.world_pos.valid = true;
		extra_data.world_pos.p1 = p1;
		extra_data.world_pos.p2 = p2;
		extra_data.world_pos.p3 = p3;
	}

	// 进行背面剔除(点的排列必须为左手螺旋后法向量朝外)
	vector_t p1_p2 = p2 - p1, p1_p3 = p3 - p1;
	vector_t v_normal = vector_cross(p1_p3, p1_p2);
	vector_t v_view = this->camera->pos - p1;
	if (features[RENDER_FEATURE_BACK_CULLING]) {
		float backCull_jug = vector_dot(v_normal, v_view);
		if (backCull_jug < 0.0f)return 1;
	}
	if (features[RENDER_FEATURE_FACK_CULLING]) {
		float backCull_jug = vector_dot(v_normal, v_view);
		if (backCull_jug > 0.0f)return 1;
	}

	if (features[RENDER_FEATURE_AUTO_NORMAL]) {
		v1.normal = v_normal;
		v2.normal = v_normal;
		v3.normal = v_normal;
	}

	//光追
	if (features[RENDER_FEATURE_RAY_TRACING]) {
		v1.pos = (v1.pos) * this->transform.model;
		v2.pos = (v2.pos) * this->transform.model;
		v3.pos = (v3.pos) * this->transform.model;
		triangle_t triangle = { v1,v2,v3 };
		this->triangles.push_back(triangle);
		return 0;
	}

	VS(&v1, &v2, &v3);
	
	//得到屏幕坐标
	p1 = v1.pos;
	p2 = v2.pos;
	p3 = v3.pos;

	p1 = viewport_transform(p1, this->transform);
	p2 = viewport_transform(p2, this->transform);
	p3 = viewport_transform(p3, this->transform);


	// 线框绘制
	if (this->render_state == RENDER_STATE_WIREFRAME) {
		//std::cout << "正在绘制线框\n";
		this->draw_line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, this->foreground);
		this->draw_line((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y, this->foreground);
		this->draw_line((int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, this->foreground);
	}
	else if(this->render_state) {
		v1.pos = p1; 
		v2.pos = p2;
		v3.pos = p3;
		vertex_set_rhw(&v1);
		vertex_set_rhw(&v2);
		vertex_set_rhw(&v3);
		Set_ExtraData_rhw(&extra_data, v1, v2, v3);
		if (render_shader_state == RENDER_SHADER_PIXEL_SCANLINE) {
			//default
			if (features[RENDER_FEATURE_SHADOW]) { this->draw_triangle(v1, v2, v3, extra_data); }
			else { this->draw_triangle(v1, v2, v3); }
		}
		else if (render_shader_state == RENDER_SHADER_PIXEL_BOUNDINGBOX) {
			this->draw_triangle_BoundingBox(v1, v2, v3);
		}
	}

	return 0;
}

int Renderer::Rendering()
{
	if (features[RENDER_FEATURE_RAY_TRACING]) {
		Rendering_RayTracing();
	}

	return 0;
}

int Renderer::Rendering_RayTracing()
{
	color_t color_tmp = { 0,0,0,1 }, color = { 0,0,0,1 };
	matrix_t camera_to_world = matrix_get_inverse(this->transform.view);
	float scale = tan(radians(camera->fov * 0.5f));
	float aspect = width / height;

	auto getFilePath = []()->std::string {
		return "D:\\OneDrive\\Program\\Arithmetic\\test\\2022校赛\\";
	};
	std::ifstream inFile;
	inFile.open((getFilePath() + "randomTriangleCnt.txt").c_str(), std::ios::in);
	int cnt; inFile >> cnt;
	std::string outFile = "\0";
	outFile += getFilePath();
	outFile += std::to_string(cnt - 1); outFile += ".in";
	std::ofstream outputFile;
	outputFile.open(outFile.c_str(), std::ios::app);
	outputFile << height * width * 2 * 2 * raytracing_samples_num << "\n";
	outputFile << std::fixed << std::setprecision(5);

	for (unsigned int y = 0; y < height; y++) {
		//fprintf(stderr, "\rRendering (%d spp) %5.2f%%", raytracing_samples_num * 4, 100.0f * y / (height - 1.0f));
		for (unsigned int x = 0; x < width; x++) {
			color = { 0,0,0,1 };
			for (unsigned sy = 0; sy < 2; sy++) { // 2x2子像素
				for (unsigned int sx = 0; sx < 2; sx++) {
					color_tmp = { 0,0,0,1 };
					for (int s = 0; s < raytracing_samples_num; s++) {
						float dx = rand_lr(0.0f, 1.0f) - 0.5f;//随机偏移
						float dy = rand_lr(0.0f, 1.0f) - 0.5f;

						float px = ((2 * (x + 0.5 + (sx - 0.5 + dx)) / width - 1)) * aspect * scale;
						float py = ((1 - 2 * (y + 0.5 + (sy - 0.5 + dy)) / height)) * scale;
						point_t pixel_pos = vector_t(px, py, 0.76, 1) * camera_to_world;

						outputFile << pixel_pos.x << " " << pixel_pos.y << " " << pixel_pos.z << "\n";

						vector_t view_dir = vector_normalize(pixel_pos - camera->pos);

						float tmp_f = 0.0f;
						color_tmp = color_tmp +
							Ray_Tracing({ camera->pos, view_dir }, 0, tmp_f) * (1.0 / raytracing_samples_num);
					}
					color = color + color_tmp * 0.25;
				}
			}

			this->draw_pixel(x, y, color_trans_255(color));
		}
	}

	inFile.close();
	outputFile.close();
	std::cout << "output pixel pos done\n";

	return 0;
}

color_t Renderer::Ray_Tracing(const ray_t& ray, int depth, float& dis)
{
	if (features[RENDER_FEATURE_RAY_TRACING_PBR]) {
		return Radiance(ray, depth);
	}

	if (++depth > raytracing_max_depth) {
		return { 0,0,0,1 };
	}
	const float inf = 1e10;

	//计算与射线相交的最近三角形
	float t = inf, cal_t = inf;
	bool is_intersect = false;
	triangle_t triangle;
	for (unsigned int i = 0; i < triangles.size(); i++) {
		if (Intersect(ray, cal_t, triangles[i]) && cal_t > 0.0f) { //有交点并且交点在视线前方
			if (cal_t < t) {
				is_intersect = true;
				t = cal_t;
				triangle = triangles[i];
			}
		}
	}
	if (!is_intersect) { 
		if (depth <= 1) { return this->background_f; }
		return {0,0,0,1}; 
	}

	point_t p_intersect = ray.o + ray.dir * t;
	dis = t;
	barycentric_t bary = Get_Barycentric(p_intersect, triangle.v1.pos, triangle.v2.pos, triangle.v3.pos);
	vertex_t v = triangle.v1 * bary.w1 + triangle.v2 * bary.w2 + triangle.v3 * bary.w3;

	color_t color = { 0,0,0,1 };
	if (render_state == RENDER_STATE_TEXTURE) { v.color = { 1,1,1,1 }; }

	for (auto& light : this->lights) {
		ray_t ray_to_light(p_intersect, vector_normalize(light->pos - p_intersect));
		float dis_p_light = vector_length(light->pos - p_intersect);

		float t = inf, cal_t = inf;
		bool is_intersect = false;
		triangle_t triangle_intersect;
		for (auto& tri : triangles) {
			if (Intersect(ray_to_light, cal_t, tri) && cal_t > 0.0f && cal_t < dis_p_light) {//在光源后的交点也忽略
				if (cal_t < t) {
					is_intersect = true;
					t = cal_t;
					triangle_intersect = tri;
				}
			}
		}
		if (!is_intersect) { 
			color += Calculate_Lighting(v, light); 
		}
		else {
			ray_t r_reflect(p_intersect, vector_normalize(vector_reflect(ray.dir, v.normal)));

			float distance = vector_length(light->pos - v.pos);
			float attenuation = 1.0f / (light->constant + light->linear * distance +
				light->quadratic * (distance * distance));
			color += light->ambient * attenuation * v.color;

			float dis_reflect = 0.0f;
			color += Ray_Tracing(r_reflect, depth, dis_reflect) * 0.1f;
		}
	}

	if (render_state == RENDER_STATE_TEXTURE) {
		color = color * texture->Read(v.tex.u, v.tex.v);
	}

	return color;
}

color_t Renderer::Radiance(const ray_t& ray, int depth)
{
	const float inf = 1e10;

	//计算与射线相交的最近三角形
	float t = inf, cal_t = inf;
	bool is_intersect = false;
	triangle_t triangle;
	for (unsigned int i = 0; i < triangles.size(); i++) {
		if (Intersect(ray, cal_t, triangles[i]) && cal_t >= 0.0001f) { //有交点并且交点在视线前方
			if (cal_t < t) {
				is_intersect = true;
				t = cal_t;
				triangle = triangles[i];
			}
		}
	}
	if (!is_intersect) {
		if (depth <= 0) { return this->background_f; }
		return { 0,0,0,1 };
	}

	//计算相交点的相关变量
	point_t p_intersect = ray.o + ray.dir * t;
	barycentric_t bary = Get_Barycentric(p_intersect, triangle.v1.pos, triangle.v2.pos, triangle.v3.pos);
	vertex_t vt = triangle.v1 * bary.w1 + triangle.v2 * bary.w2 + triangle.v3 * bary.w3;
	vector_t N = vector_normalize(vt.normal), NL = vector_dot(N, ray.dir) < 0 ? N : N * -1;
	color_t f = vt.color;
	float p = std::max(f.r, std::max(f.g, f.b));

	//俄罗斯转盘,随机继续迭代
	if (++depth > raytracing_max_depth) {
		if (rand_lr(0.0f, 1.0f) < p) {
			f = f * (1.0f / p);
		}
		else {
			return vt.emissivity;
		}
	}

	//渲染方程
	if (vt.material.reflect == material_t::reflect_t::DIFF) { //diffuse漫反射
		float r1 = 2 * PI * rand_lr(0.0f, 1.0f);
		float r2 = rand_lr(0.0f, 1.0f), r2s = sqrt(r2);
		vector_t w = NL;
		vector_t u = vector_normalize(vector_cross((abs(w.x) > 0.1f) ? vector_t(0, 1.0f, 0) : vector_t(1.0f, 0, 0), w));
		vector_t v = vector_cross(w, u);
		vector_t d = vector_normalize(u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2));
		color_t color_radiance = Radiance(ray_t(p_intersect, d), depth);
		f.r *= color_radiance.r; f.g *= color_radiance.g; f.b *= color_radiance.b;

		return vt.emissivity + f;
	}
	else if (vt.material.reflect == material_t::reflect_t::SPEC) { //specular镜面反射
		color_t color_radiance = Radiance(ray_t(p_intersect, vector_normalize(vector_reflect(ray.dir, N))), depth);
		f.r *= color_radiance.r; f.g *= color_radiance.g; f.b *= color_radiance.b;
		return vt.emissivity + f;
	}
	else if (vt.material.reflect == material_t::reflect_t::REFR) {
		//todo
		ray_t ray_reflect(p_intersect, vector_normalize(vector_reflect(ray.dir, N)));
	}
}

void Renderer::VS(vertex_t * v1, vertex_t * v2, vertex_t * v3)
{
	//光照处理
	if (this->render_state == RENDER_STATE_TEXTURE) {
		v1->color = v2->color = v3->color = { 1,1,1,1 };
	}
	color_t color1, color2, color3;
	if (this->features[RENDER_FEATURE_LIGHT] && !features[RENDER_FEATURE_LIGHT_PHONG]) {
		color1 = color2 = color3 = { 0,0,0,0 };
		for (auto& light : lights) {
			color1 += Calculate_Lighting(*v1, light);
			color2 += Calculate_Lighting(*v2, light);
			color3 += Calculate_Lighting(*v3, light);
		}
		v1->color = color1;
		v2->color = color2;
		v3->color = color3;
	}

	v1->pos = v1->pos * this->transform.model;
	v2->pos = v2->pos * this->transform.model;
	v3->pos = v3->pos * this->transform.model;

	v1->pos_world = v1->pos;
	v2->pos_world = v2->pos;
	v3->pos_world = v3->pos;

	/* 将点映射到观察空间 */
	v1->pos = v1->pos * this->transform.view;
	v2->pos = v2->pos * this->transform.view;
	v3->pos = v3->pos * this->transform.view;

	/* 将点映射到裁剪空间 */
	v1->pos = v1->pos * this->transform.projection;
	v2->pos = v2->pos * this->transform.projection;
	v3->pos = v3->pos * this->transform.projection;
}

vertex_t Renderer::Split_Triangle(const vertex_t* v1, const vertex_t* v2, const vertex_t* v3, float t)
{
	return vertex_t();
}

color_t Renderer::PS_Interpolation(vertex_t* v1, vertex_t* v2, vertex_t* v3, barycentric_t bary)
{
	vertex_t v = (*v1) * bary.w1 + (*v2) * bary.w2 + (*v3) * bary.w3;
	auto tmp_rhw = v.rhw;
	v = v * (1.0 / v.rhw);
	v.rhw = tmp_rhw;

	return PS(&v);
}

color_t Renderer::PS(vertex_t* v)
{
	//阴影
	bool pixel_in_shadow = false;
	if (features[RENDER_FEATURE_SHADOW] == true) {
		float bias = 0.1f;
		for (const Light* light : this->lights) {
			point_t p = v->pos_world;
			p = p * light->light_space_matrix;
			p = viewport_transform(p, this->transform);

			//u,v归一化
			float world_u = p.x / this->width;
			float world_v = p.y / this->height;
			if (world_u > 1 || world_u < 0
				|| world_v > 1 || world_v < 0)
			{
				continue;
			}

			float world_map_deep = light->shadow_map->Read(world_u, world_v, 0).r;
			if (p.w - bias > world_map_deep) {
				pixel_in_shadow = true;
			}
		}
	}

	color_t color_use = v->color;
	//逐片元处理光照
	if (features[RENDER_FEATURE_LIGHT]) {
		if (features[RENDER_FEATURE_LIGHT_PHONG]) {

			vertex_t v_world = *v;
			v_world.pos = v->pos_world;

			Phong_Shading(v_world, pixel_in_shadow);

			color_use = v_world.color;
		}
		else {
			// 高洛德着色(Gouraud Shading)的阴影处理
			if (pixel_in_shadow) {
				color_use *= 0.5;
			}
		}
	}

	if (this->render_state == RENDER_STATE_COLOR) {
		color_use = color_use;
	}
	else if (this->render_state == RENDER_STATE_TEXTURE) {
		color_use = color_use * texture->Read(v->tex.u, v->tex.v);
	}
	else if (this->render_state == RENDER_STATE_DEEP) {
		color_t color_deep;
		color_deep.r = 1 / v->rhw;
		while (color_deep.r < 1) { color_deep.r *= 10; }
		while (color_deep.r > 1) { color_deep.r /= 10; }
		color_deep.g = color_deep.b = color_deep.r;
		color_use = color_deep;
	}

	return color_use;
}


void Renderer::transform_update()
{
	this->transform.transform = transform.model * transform.view * transform.projection;
}

void Renderer::FXAA(bool on)
{
	if (!on)return;
	color_t color;
	for (int j = 1; j < height-1; j++) {
		for (int i = 1; i < width-1; i++) {
			int cnt = 4;
			color_t color_cur = color_trans_1f(frame_buffer[j][i])*4;
			color = color_cur;
			for (int jj = -1; jj <= 1; jj++) {
				for (int ii = -1; ii <= 1; ii++) {
					//float delta = 0;
					//color_t color_aro = color_trans_1f(frame_buffer[j + jj][i + ii]);
					//delta += abs(color_cur.r - color_aro.r);
					//delta += abs(color_cur.g - color_aro.g);
					//delta += abs(color_cur.b - color_aro.b);
					//if (delta >= 0.75f) {
						color += color_trans_1f(frame_buffer[j + jj][i + ii]);
						cnt++;
					//}
				}
			}
			color = color / cnt;
			frame_buffer[j][i] = color_trans_255(color);
		}
	}
}
