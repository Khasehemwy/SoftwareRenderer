#pragma once
#include"Includes.h"


//渲染模式
#define RENDER_STATE_WIREFRAME 1	// 渲染线框
#define RENDER_STATE_COLOR 2	//渲染颜色

//特性
#define RENDER_FEATURE_BACK_CULLING 1	//背面剔除

//片段着色器-着色算法
#define RENDER_SHADER_PIXEL_SCANLINE 1	//扫描线算法-进行片段着色,更快但不够精准
#define RENDER_SHADER_PIXEL_BOUNDINGBOX 2	//边界盒算法-进行片段着色,精准但不快

class Renderer
{
public:
	transform_t transform;      // 坐标变换器
	const Camera* camera = nullptr;
	float width, height;

	UINT32** frame_buffer;		// 像素缓存：framebuffer[y] 代表第 y行
	float** z_buffer;			// 深度缓存：zbuffer[y] 为第 y行指针

	UINT32 background;			// 背景颜色
	UINT32 foreground;			// 线框颜色

	int render_state = RENDER_STATE_WIREFRAME;	// 渲染状态
	int render_shader_state = RENDER_SHADER_PIXEL_SCANLINE;	// 片段着色器算法选择

	std::map<int, int> features;	//特性

	Renderer();

	void init(int width, int height, void* fb);
	void destroy();
	void clear();

	void draw_pixel(int x, int y, UINT32 color);
	void draw_line(int x1, int y1, int x2, int y2, UINT32 color);
	void draw_triangle(vertex_t v1, vertex_t v2, vertex_t v3);
	void draw_triangle_StandardAlgorithm(const vertex_t& top, const vertex_t& left, const vertex_t& right);
	void draw_triangle_BresenhamAlgorithm(const vertex_t& top, const vertex_t& left, const vertex_t& right);
	void draw_triangle_BoundingBox(const vertex_t& v1, const vertex_t& v2, const vertex_t& v3);
	int display_primitive(const vertex_t& v1, const vertex_t& v2, const vertex_t& v3);
	void transform_update();
};

