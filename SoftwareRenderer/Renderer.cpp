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

//经典算法,有缺陷
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

	if (y1 == y2) {
		//平顶三角
		if (x1 > x2)std::swap(v1, v2);
		draw_triangle_BresenhamAlgorithm(v3, v1, v2);
		return;
	}
	if (y2 == y3) {
		//平底三角
		if (x2 > x3)std::swap(v2, v3);
		draw_triangle_BresenhamAlgorithm(v1, v2, v3);
		return;
	}

	//拆分三角
	vertex_t v4 = v2;
	float dxy = (x3 - x1) / (y3 - y1);
	v4.pos.x = (y2 - y1) * dxy + x1;
	float x4 = v4.pos.x, y4 = y2;
	if (x2 <= x4) {
		draw_triangle_BresenhamAlgorithm(v1, v2, v4);
		draw_triangle_BresenhamAlgorithm(v3, v2, v4);
	}
	else {
		draw_triangle_BresenhamAlgorithm(v1, v4, v2);
		draw_triangle_BresenhamAlgorithm(v3, v4, v2);
	}
}

void Renderer::draw_triangle_StandardAlgorithm(const vertex_t& top, const vertex_t& left, const vertex_t& right)
{
	float dxy_left = (top.pos.x - left.pos.x) / (top.pos.y - left.pos.y);
	float dxy_right = (top.pos.x - right.pos.x) / (top.pos.y - right.pos.y);
	float xs, xe;
	int y0 = (int)(top.pos.y + 0.5f);
	int y1 = (int)(left.pos.y + 0.5f);

	//TODO
	//颜色插值
	float r, g, b, a;
	r = top.color.r;
	g = top.color.g;
	b = top.color.b;
	int R = (int)(r * 255.0f);
	int G = (int)(g * 255.0f);
	int B = (int)(b * 255.0f);
	R = CMID(R, 0, 255);
	G = CMID(G, 0, 255);
	B = CMID(B, 0, 255);
	UINT color = (R << 16) | (G << 8) | (B);

	//绘制平底或平顶三角形
	if (y0 <= y1) {
		//平底
		xs = xe = top.pos.x;
		for (int y = y0; y <= y1; y++) {
			this->draw_line(xs + 0.5f, y, xe + 0.5f, y, color);
			xs += dxy_left;
			xe += dxy_right;
		}
	}
	else {
		//平顶
		xs = left.pos.x;// +(ceil(y1) - y1) * dxy_left;
		xe = right.pos.x;// +(ceil(y1) - y1) * dxy_right;
		for (int y = y1; y <= y0; y++) {
			this->draw_line(xs + 0.5f, y, xe + 0.5f, y, color);
			xs += dxy_left;
			xe += dxy_right;
		}
	}
}

void Renderer::draw_triangle_BresenhamAlgorithm(const vertex_t& top, const vertex_t& left, const vertex_t& right)
{
	int x1_s = top.pos.x, y1_s = top.pos.y;
	int x1_e = top.pos.x, y1_e = top.pos.y;
	int x2 = left.pos.x, y2 = left.pos.y;
	int x3 = right.pos.x, y3 = right.pos.y;

	//TODO
	//颜色插值
	float r, g, b, a;
	r = top.color.r;
	g = top.color.g;
	b = top.color.b;
	int R = (int)(r * 255.0f);
	int G = (int)(g * 255.0f);
	int B = (int)(b * 255.0f);
	R = CMID(R, 0, 255);
	G = CMID(G, 0, 255);
	B = CMID(B, 0, 255);
	UINT color = (R << 16) | (G << 8) | (B);

	int dx_s = std::abs(x1_s - x2), dx_e = std::abs(x1_e - x3);
	int dy = std::abs(y1_s - y2);
	int y_s = min(y1_s, y2);
	int y_e = y_s;
	int rem_s = 0, rem_e = 0;
	int x_s, x_e;

	this->draw_pixel(x1_s, y1_s, color);
	this->draw_pixel(x2, y2, color);
	this->draw_pixel(x3, y3, color);

	//left edge
	if (y2 < y1_s) { std::swap(y1_s, y2); std::swap(x1_s, x2); }
	x_s = x1_s; y_s = y1_s;
	//right edge
	if (y3 < y1_e) { std::swap(y1_e, y3); std::swap(x1_e, x3); }
	x_e = x1_e; y_e = y1_e;

	while ((y_s <= y2 && x_s >= min(x1_s, x2) && x_s <= max(x1_s, x2))
		|| (y_e <= y2 && x_e >= min(x1_e, x3) && x_e <= max(x1_e, x3)))
	{
		if (y_s == y_e) {
			this->draw_line(x_s, y_s, x_e, y_e, color);
		}
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
			this->draw_pixel(x_s, y_s, color);
			if (dx_s > dy) { x_s += (x2 > x1_s) ? 1 : -1; }
			else { y_s++; }
		}
		if (y_s == y_e) {
			this->draw_line(x_s, y_s, x_e, y_e, color);
		}
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
			this->draw_pixel(x_e, y_e, color);
			if (dx_e > dy) { x_e += (x3 > x1_e) ? 1 : -1; }
			else { y_e++; }
		}
	}

}

//绘制原始三角形
int Renderer::display_primitive(const vertex_t& v1, const vertex_t& v2, const vertex_t& v3)
{
	point_t p1, p2, p3;

	// 将点映射到世界空间,进行背面剔除
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
		this->draw_triangle(v1_tmp, v2_tmp, v3_tmp);
	}

	return 0;
}



void Renderer::transform_update()
{
	this->transform.transform = transform.model * transform.view * transform.projection;
}
