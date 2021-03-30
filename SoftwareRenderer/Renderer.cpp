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

	this->frame_buffer = (UINT**)ptr;
	this->z_buffer = (float**)(ptr + sizeof(void*) * height);
	ptr += sizeof(void*) * height * 2;
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

	transform_init(&this->transform, width, height);
	this->render_state = RENDER_STATE_WIREFRAME;
}

void Renderer::destroy()
{
	if (this->frame_buffer)
		free(this->frame_buffer);
	this->frame_buffer = nullptr;
	this->z_buffer = nullptr;
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
	for (; y <= y2 && x >= min(x1, x2) && x <= max(x1, x2);) {
		if (dx > dy) {
			rem += dy;
			if (rem >= dx) {
				rem -= dx;
				y++;
			}
		}
		else {
			rem += dx;
			if (rem >= dy) {
				rem -= dy;
				x += (x2 > x1) ? 1 : -1;
			}
		}
		this->draw_pixel(x, y, color);
		if (dx > dy) { x += (x2 > x1) ? 1 : -1; }
		else { y++; }
	}
	/* //和上分结果相同,比较次数更少
	* //直线可能从上往下,可能从左往右
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
	}*/ //

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
	//计算v4的颜色
	float t = (y4 - y1) / (y3 - y1);
	v4.color = v1.color + (v3.color - v1.color) * t;

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
	float dxy_left = (top.pos.x - left.pos.x) / (top.pos.y - left.pos.y);
	float dxy_right = (top.pos.x - right.pos.x) / (top.pos.y - right.pos.y);
	float xs, xe;
	int y0 = (int)(ceil(top.pos.y));
	int y1 = (int)(ceil(left.pos.y));

	//颜色插值
	//左侧颜色强度梯度(y增加1时,颜色增加dildy)
	color_t dildy = (left.color - top.color) / (left.pos.y - top.pos.y);
	//右侧
	color_t dirdy = (right.color - top.color) / (left.pos.y - top.pos.y);

	//绘制平底或平顶三角形
	if (y0 <= y1) {
		//平底三角形
		//x值修正
		xs = top.pos.x + (ceil(top.pos.y) - top.pos.y) * dxy_left;
		xe = top.pos.x + (ceil(top.pos.y) - top.pos.y) * dxy_right;
		//颜色
		//颜色也同样因为浮点数转换整数需要修正
		color_t color_left = top.color + (ceil(top.pos.y) - top.pos.y) * dildy;
		color_t color_right = top.color + (ceil(top.pos.y) - top.pos.y) * dirdy;

		for (int y = y0; y < y1; y++) {
			color_t dix = (color_right - color_left) / (xe - xs);
			color_t color = color_left + (ceil(xs) - xs) * dix;
			for (int x = ceil(xs); x <= ceil(xe); x++) {
				this->draw_pixel(x, y, color_trans_255(color));
				color = color + dix;
			}
			xs += dxy_left;
			xe += dxy_right;
			color_left = color_left + dildy;
			color_right = color_right + dirdy;
		}
	}
	else {
		//平顶,类似平底三角形
		xs = left.pos.x + (ceil(left.pos.y) - left.pos.y) * dxy_left;
		xe = right.pos.x + (ceil(right.pos.y) - right.pos.y) * dxy_right;
		//颜色
		color_t color_left = left.color + (ceil(left.pos.y) - left.pos.y) * dildy;
		color_t color_right = right.color + (ceil(right.pos.y) - right.pos.y) * dirdy;

		for (int y = y1; y < y0; y++) {
			color_t dix = (color_right - color_left) / (xe - xs);
			color_t color = color_left + (ceil(xs) - xs) * dix;
			for (int x = ceil(xs); x <= ceil(xe); x++) {
				this->draw_pixel(x, y, color_trans_255(color));
				color = color + dix;
			}
			xs += dxy_left;
			xe += dxy_right;
			color_left = color_left + dildy;
			color_right = color_right + dirdy;
		}
	}
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

	// 将点映射到世界空间,进行背面剔除(点的排列必须为右手螺旋后法向量朝外)
	p1 = (v1.pos) * this->transform.model;
	p2 = (v2.pos) * this->transform.model;
	p3 = (v3.pos) * this->transform.model;

	if (features[RENDER_FEATURE_BACK_CULLING]) {
		vector_t p1_p2 = p2 - p1, p1_p3 = p3 - p1;
		vector_t v_normal = vector_cross(p1_p2, p1_p3);
		vector_t v_view = p1 - this->camera->camera_pos;

		float backCull_jug = v_normal * v_view;
		if (backCull_jug <= 0.0f)return 1;
	}


	//将点映射到裁剪空间
	p1 = p1 * this->transform.view * this->transform.projection;
	p2 = p2 * this->transform.view * this->transform.projection;
	p3 = p3 * this->transform.view * this->transform.projection;

	//裁剪检测
	int cvv_jug = 0;
	if (check_cvv(p1) != 0)cvv_jug = 1;
	if (check_cvv(p2) != 0)cvv_jug = 1;
	if (check_cvv(p3) != 0)cvv_jug = 1;
	if (cvv_jug) {
		//std::cout << "cvv cut\n";
		return 1;
	}

	//得到屏幕坐标
	p1 = viewport_transform(p1, this->transform);
	p2 = viewport_transform(p2, this->transform);
	p3 = viewport_transform(p3, this->transform);

	//TODO
	//纹理或者色彩绘制
	//draw_triangle(v1, v2, v3);

	// 线框绘制
	if (this->render_state == RENDER_STATE_WIREFRAME) {
		this->draw_line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, this->foreground);
		this->draw_line((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y, this->foreground);
		this->draw_line((int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, this->foreground);
	}
	else if (this->render_state == RENDER_STATE_COLOR) {
		vertex_t v1_tmp, v2_tmp, v3_tmp;
		v1_tmp = v1; v2_tmp = v2; v3_tmp = v3;
		v1_tmp.pos = p1; 
		v2_tmp.pos = p2;
		v3_tmp.pos = p3;
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
