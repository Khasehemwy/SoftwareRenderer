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

//经典算法,有缺陷
void Renderer::draw_triangle_StandardAlgorithm(vertex_t v1, vertex_t v2, vertex_t v3)
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
		draw_triangle_flat(v3, v1, v2);
		return;
	}
	if (y2 == y3) {
		//平底三角
		if (x2 > x3)std::swap(v2, v3);
		draw_triangle_flat(v1, v2, v3);
		return;
	}

	//拆分三角
	vertex_t v4 = v2;
	float dxy = (x3 - x1) / (y3 - y1);
	v4.pos.x = (y2 - y1) * dxy + x1;
	float x4 = v4.pos.x, y4 = y2;
	if (x2 <= x4) {
		draw_triangle_flat(v1, v2, v4);
		draw_triangle_flat(v3, v2, v4);
	}
	else {
		draw_triangle_flat(v1, v4, v2);
		draw_triangle_flat(v3, v4, v2);
	}
}

void Renderer::draw_triangle_flat(vertex_t top, vertex_t left, vertex_t right)
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
			//std::cout << "?";
			this->draw_line(xs + 0.5f, y, xe + 0.5f, y, color);
			xs += dxy_left;
			xe += dxy_right;
		}
	}
	else {
		//平顶
		xs = left.pos.x; xe = right.pos.x;
		for (int y = y1; y <= y0; y++) {
			this->draw_line(xs + 0.5f, y, xe + 0.5f, y, color);
			xs += dxy_left;
			xe += dxy_right;
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
		this->draw_triangle_StandardAlgorithm(v1_tmp, v2_tmp, v3_tmp);
	}

	return 0;
}



void Renderer::transform_update()
{
	this->transform.transform = transform.model * transform.view * transform.projection;
}
