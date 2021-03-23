#pragma once
#include"Includes.h"

#define RENDER_STATE_WIREFRAME      1		// 渲染线框

class Renderer
{
public:
	transform_t transform;      // 坐标变换器
	float width, height;

	UINT32** frame_buffer;		// 像素缓存：framebuffer[y] 代表第 y行
	float** z_buffer;			// 深度缓存：zbuffer[y] 为第 y行指针

	UINT32 background;			// 背景颜色
	UINT32 foreground;			// 线框颜色

	int render_state = RENDER_STATE_WIREFRAME;           // 渲染状态

	void init(int width, int height, void* fb);
	void destroy();
	void clear();

	void draw_pixel(int x, int y, UINT32 color);
	void draw_line(int x1, int y1, int x2, int y2, UINT32 color);
	int display_primitive(const vertex_t& v1, const vertex_t& v2, const vertex_t& v3);
	void transform_update();
};

