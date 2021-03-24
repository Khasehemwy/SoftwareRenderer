#include "Renderer.h"

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

void Renderer::draw_line(int x1, int y1, int x2, int y2, UINT32 color)
{
	int dx = std::abs(x1 - x2);
	int dy = std::abs(y1 - y2);
	int rem = 0;
	if (dx > dy) {
		if (x2 < x1) { std::swap(x1, x2); std::swap(y1, y2); }
		for (int x = x1, y = y1; x <= x2; x++) {
			rem += dy;
			if (rem >= dx) {
				rem -= dx;
				//this->draw_pixel(x, y, color);
				y += (y2 > y1) ? 1 : -1;
				//this->draw_pixel(x-1, y, color);
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
				//this->draw_pixel(x, y, color);
				x += (x2 > x1) ? 1 : -1;
				//this->draw_pixel(x, y-1, color);
			}
			this->draw_pixel(x, y, color);
		}
	}
	this->draw_pixel(x1, y1, color);
	this->draw_pixel(x2, y2, color);
}

//绘制原始三角形
int Renderer::display_primitive(const vertex_t& v1, const vertex_t& v2, const vertex_t& v3)
{
	point_t p1, p2, p3;

	// 将点映射到裁剪空间
	p1 = (v1.pos) * (this->transform);
	p2 = (v2.pos) * (this->transform);
	p3 = (v3.pos) * (this->transform);

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

	// 线框绘制
	if (this->render_state == RENDER_STATE_WIREFRAME) {
		this->draw_line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, this->foreground);
		this->draw_line((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y, this->foreground);
		this->draw_line((int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, this->foreground);
	}

	return 0;
}

void Renderer::transform_update()
{
	this->transform.transform = transform.model * transform.view * transform.projection;
}
