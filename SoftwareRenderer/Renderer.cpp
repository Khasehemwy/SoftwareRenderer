#include "Renderer.h"

Renderer::Renderer()
{
	features[RENDER_FEATURE_BACK_CULLING] = true;
	features[RENDER_FEATURE_LIGHT] = true;
	features[RENDER_FEATURE_CVV_CLIP] = true;
	features[RENDER_FEATURE_SHADOW] = true;
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
	//基于y的梯度,y增加1时,对应x/u/v/i/rhw增加的值
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

	int y0 = 0, y1 = 0;
	float y0f = top.pos.y, y1f = left.pos.y;
	//赋初始值,暂不修正
	//平底或平顶三角形
	if (y0f <= y1f) {
		/*平底三角形*/
		xl = top.pos.x;
		xr = top.pos.x;
		//纹理
		ul = top.tex.u;
		vl = top.tex.v;
		ur = top.tex.u;
		vr = top.tex.v;
		//颜色
		color_left = top.color;
		color_right = top.color;
		//深度
		rhwl = top.rhw;
		rhwr = top.rhw;
	}
	else {
		/*平顶三角形,类似平底三角形*/
		std::swap(y0, y1);
		std::swap(y0f, y1f);
		xl = left.pos.x;
		xr = right.pos.x;
		ul = left.tex.u;
		vl = left.tex.v;
		ur = right.tex.u;
		vr = right.tex.v;
		color_left = left.color;
		color_right = right.color;
		rhwl = left.rhw;
		rhwr = right.rhw;
	}

	//垂直裁剪
	if (y1f < min_clip_y) { return; }
	if (y0f < min_clip_y) {
		float dy = min_clip_y - y0f;
		xl += dxdy_l * dy;
		xr += dxdy_r * dy;
		ul += dudy_l * dy;
		vl += dvdy_l * dy;
		ur += dudy_r * dy;
		vr += dvdy_r * dy;
		color_left += didy_l * dy;
		color_right += didy_r * dy;
		rhwl += drhwdy_l * dy;
		rhwr += drhwdy_r * dy;
		y0f = min_clip_y;
	}
	y0 = (int)(ceil(y0f));
	y1 = (int)(ceil(y1f));
	//修正(浮点数转换整数需要修正)
	float delta = y0 - y0f;
	xl += delta * dxdy_l;
	xr += delta * dxdy_r;
	ul += delta * dudy_l;
	vl += delta * dvdy_l;
	ur += delta * dudy_r;
	vr += delta * dvdy_r;
	color_left += delta * didy_l;
	color_right += delta * didy_r;
	rhwl += delta * drhwdy_l;
	rhwr += delta * drhwdy_r;



	//从上往下绘制
	for (int y = y0; y < y1; y++) {
		if (y >= max_clip_y)break;
		float dx = xr - xl;
		float dux = (ur - ul) / dx;
		float dvx = (vr - vl) / dx;
		float drhwdx = (rhwr - rhwl) / dx;
		float ui = ul;
		float vi = vl;
		float rhwi = rhwl;
		color_t dix = (color_right - color_left) / dx;
		color_t color = color_left + (ceil(xl) - xl) * dix;

		float xli = xl, xri = xr;
		//调整到下一步
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

		//水平裁剪
		if (xri < min_clip_x) { continue; }
		if (xli < min_clip_x) {
			float dx = min_clip_x - xli;
			color += dix * dx;
			ui += dux * dx;
			vi += dvx * dx;
			rhwi += drhwdx * dx;
			xli = min_clip_x;
		}

		//修正
		int x0 = (int)ceil(xli);
		int x1 = (int)ceil(xri);
		float delta = x0 - xli;
		color += delta * dix;
		ui += delta * dux;
		vi += delta * dvx;
		rhwi += delta * drhwdx;

		for (int x = x0; x <= x1; x++) {
			if (x >= max_clip_x)break;
			if (x >= 0 && y >= 0) {
				if (rhwi >= this->z_buffer[y][x]) {
					this->z_buffer[y][x] = rhwi;// 深度缓存
					float wi = 1.0 / rhwi;
					if (this->render_state == RENDER_STATE_COLOR) {
						this->draw_pixel(x, y, color_trans_255(color * wi));
					}
					if (this->render_state == RENDER_STATE_TEXTURE) {
						color_t color_tmp = color * this->texture_read(*texture, ui * wi, vi * wi);
						this->draw_pixel(x, y, color_trans_255(color_tmp * wi));
					}
					//阴影
					//if (features[RENDER_FEATURE_SHADOW] == true) {
						//point_t p_light_space;
						//p_light_space.x = x;
						//p_light_space.y = y;
						//p_light_space.w = wi;
						//p_light_space = anti_viewport_transform(p_light_space, this->transform);
						//matrix_t inv_proj = matrix_get_inverse(transform.projection);
						//matrix_t inv_view = matrix_get_inverse(transform.view);
						//p_light_space = p_light_space * inv_proj;
						//p_light_space = p_light_space * inv_view;
						//////现在是世界空间的坐标,接下来变换到光源空间
						//p_light_space = p_light_space * current_light->light_space_matrix;
						//p_light_space = viewport_transform(p_light_space, this->transform);
						//float rhwi_lt = 1.0f / p_light_space.w;
						//int x_lt = p_light_space.x;
						//int y_lt = p_light_space.y;
						//if (rhwi_lt <= current_light->shadow_map->texture[y_lt][x_lt].r) {
						//	UINT32 color_255 = frame_buffer[y][x];
						//	color_t color_f = color_trans_1f(color_255);
						//	this->draw_pixel(x, y, color_trans_255(current_light->ambient* color_f));
						//}
					//}
				}
			}
			color = color + dix;
			ui += dux;
			vi += dvx;
			rhwi += drhwdx;
		}
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
int Renderer::display_primitive(vertex_t v1, vertex_t v2, vertex_t v3)
{
	point_t p1, p2, p3;
	point_t p1_model, p2_model, p3_model;

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
	p1_model = p1;
	p2_model = p2;
	p3_model = p3;

	// 进行背面剔除(点的排列必须为左手螺旋后法向量朝外)
	vector_t p1_p2 = p2 - p1, p1_p3 = p3 - p1;
	vector_t v_normal = vector_cross(p1_p3, p1_p2);
	vector_t v_view = this->camera->pos - p1;
	if (features[RENDER_FEATURE_BACK_CULLING]) {
		float backCull_jug = vector_dot(v_normal, v_view);
		if (backCull_jug < 0.0f)return 1;
	}

	//光照处理
	if (this->render_state == RENDER_STATE_TEXTURE) {
		v1.color = v2.color = v3.color = { 1,1,1,1 };
	}
	color_t color1, color2, color3;
	if (this->features[RENDER_FEATURE_LIGHT] == true) {
		color1 = color2 = color3 = { 0,0,0,0 };
		for (auto& light : lights) {

			// 环境光
			color_t ambient1 = light->ambient * v1.color;
			color_t ambient2 = light->ambient * v2.color;
			color_t ambient3 = light->ambient * v3.color;

			// 漫反射
			// 使用兰伯特余弦定律（Lambert' cosine law）计算漫反射
			vector_t norm = vector_normalize(v_normal);
			vector_t light_dir1, light_dir2, light_dir3;
			float diff;
			if (light->light_state == LIGHT_STATE_DIRECTIONAL) {
				light_dir1 = vector_normalize(-light->direction);
				light_dir3 = light_dir2 = light_dir1;
			}
			else if (
				light->light_state == LIGHT_STATE_POINT ||
				light->light_state == LIGHT_STATE_SPOTLIGHT)
			{
				light_dir1 = vector_normalize(light->pos - p1);
				light_dir2 = vector_normalize(light->pos - p2);
				light_dir3 = vector_normalize(light->pos - p3);
			}

			diff = max(vector_dot(norm, light_dir1), 0.0f);
			color_t diffuse1 = light->diffuse * diff * v1.color;

			diff = max(vector_dot(norm, light_dir2), 0.0f);
			color_t diffuse2 = light->diffuse * diff * v2.color;

			diff = max(vector_dot(norm, light_dir3), 0.0f);
			color_t diffuse3 = light->diffuse * diff * v3.color;

			// 镜面反射
			vector_t reflect_dir1, reflect_dir2, reflect_dir3;
			vector_t view_dir1, view_dir2, view_dir3;
			reflect_dir1 = vector_reflect(-light_dir1, norm);
			reflect_dir2 = vector_reflect(-light_dir2, norm);
			reflect_dir3 = vector_reflect(-light_dir3, norm);
			view_dir1 = vector_normalize(camera->pos - p1);
			view_dir2 = vector_normalize(camera->pos - p2);
			view_dir3 = vector_normalize(camera->pos - p3);
			float shininess = 32.0f;

			float spec = pow(max(vector_dot(view_dir1, reflect_dir1), 0.0f), shininess);
			color_t specular1 = light->specular * spec * v1.color;

			spec = pow(max(vector_dot(view_dir2, reflect_dir2), 0.0f), shininess);
			color_t specular2 = light->specular * spec * v2.color;

			spec = pow(max(vector_dot(view_dir3, reflect_dir3), 0.0f), shininess);
			color_t specular3 = light->specular * spec * v3.color;

			//specular1 = specular2 = specular3 = { 0,0,0,0 };


			//光照运算
			if (light->light_state == LIGHT_STATE_POINT ||
				light->light_state == LIGHT_STATE_SPOTLIGHT)
			{
				//点光源衰减
				float distance1 = vector_length(light->pos - p1);
				float distance2 = vector_length(light->pos - p2);
				float distance3 = vector_length(light->pos - p3);
				float attenuation1 = 1.0 / (light->constant + light->linear * distance1 +
					light->quadratic * (distance1 * distance1));
				float attenuation2 = 1.0 / (light->constant + light->linear * distance2 +
					light->quadratic * (distance2 * distance2));
				float attenuation3 = 1.0 / (light->constant + light->linear * distance3 +
					light->quadratic * (distance3 * distance3));

				//聚光
				if (light->light_state == LIGHT_STATE_SPOTLIGHT) {
					float epsilon = light->cut_off - light->outer_cut_off;
					float theta1, theta2, theta3;
					float intensity1, intensity2, intensity3;
					vector_t light_direction = vector_normalize(-light->direction);

					theta1 = vector_dot(light_dir1, light_direction);
					intensity1 = CMID((theta1 - light->outer_cut_off) / epsilon, 0.0, 1.0);
					ambient1 *= intensity1; diffuse1 *= intensity1; specular1 *= intensity1;

					theta2 = vector_dot(light_dir2, light_direction);
					intensity2 = CMID((theta2 - light->outer_cut_off) / epsilon, 0.0, 1.0);
					ambient2 *= intensity2; diffuse2 *= intensity2; specular2 *= intensity2;

					theta3 = vector_dot(light_dir3, light_direction);
					intensity3 = CMID((theta3 - light->outer_cut_off) / epsilon, 0.0, 1.0);
					ambient3 *= intensity3; diffuse3 *= intensity3; specular3 *= intensity3;
				}

				color1 += (ambient1 + diffuse1 + specular1) * attenuation1;
				color2 += (ambient2 + diffuse2 + specular2) * attenuation2;
				color3 += (ambient3 + diffuse3 + specular3) * attenuation3;
			}
			else {
				color1 += (ambient1 + diffuse1 + specular1);
				color2 += (ambient2 + diffuse2 + specular2);
				color3 += (ambient3 + diffuse3 + specular3);
			}
		}
	}
	else {
		color1 = v1.color;
		color2 = v2.color;
		color3 = v3.color;
	}
	
	v1.color = color1;
	v2.color = color2;
	v3.color = color3;


	/* 将点映射到观察空间 */
	p1 = p1 * this->transform.view;
	p2 = p2 * this->transform.view;
	p3 = p3 * this->transform.view;

	/* 将点映射到裁剪空间 */
	p1 = p1 * this->transform.projection;
	p2 = p2 * this->transform.projection;
	p3 = p3 * this->transform.projection;

	//得到屏幕坐标
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
		if (render_shader_state == RENDER_SHADER_PIXEL_SCANLINE)//default
			this->draw_triangle(v1, v2, v3);
		else if (render_shader_state == RENDER_SHADER_PIXEL_BOUNDINGBOX)
			this->draw_triangle_BoundingBox(v1, v2, v3);
	}

	//if (features[RENDER_FEATURE_SHADOW] == true) {
	//	if (render_state == RENDER_STATE_COLOR ||
	//		render_state == RENDER_STATE_TEXTURE) {
	//		//处理阴影
	//		for (auto& light : this->lights) {
	//			point_t p1, p2, p3;
	//			p1 = p1_model * light->light_space_matrix;
	//			p2 = p2_model * light->light_space_matrix;
	//			p3 = p3_model * light->light_space_matrix;

	//			p1 = viewport_transform(p1, this->transform);
	//			p2 = viewport_transform(p2, this->transform);
	//			p3 = viewport_transform(p3, this->transform);
	//			vertex_t v1, v2, v3;
	//			v1.pos = p1; v2.pos = p2; v3.pos = p3;
	//			//v1.tex.u = p1.x; v1.tex.v = p1.y;
	//			//v2.tex.u = p2.x; v2.tex.v = p2.y;
	//			//v3.tex.u = p3.x; v3.tex.v = p3.y;
	//			vertex_set_rhw(&v1);
	//			vertex_set_rhw(&v2);
	//			vertex_set_rhw(&v3);
	//			current_light = light;
	//			this->draw_triangle_shadow(v1, v2, v3);
	//		}
	//	}
	//}

	return 0;
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
