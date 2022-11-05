#pragma once
#include"Includes.h"


//渲染模式
#define RENDER_STATE_WIREFRAME 1		// 渲染线框
#define RENDER_STATE_COLOR 2			// 渲染颜色
#define RENDER_STATE_TEXTURE 3			// 渲染纹理
#define RENDER_STATE_DEEP 4				// 渲染纹理

//特性
#define RENDER_FEATURE_BACK_CULLING 1		//背面剔除
#define RENDER_FEATURE_FACK_CULLING 2		//正面剔除
#define RENDER_FEATURE_LIGHT 3				//是否开启光照(默认高洛德着色)
#define RENDER_FEATURE_LIGHT_PHONG 4		//是否开启光照的冯氏着色
#define RENDER_FEATURE_CVV_CLIP 5			//是否开启cvv裁剪
#define RENDER_FEATURE_SHADOW 6				//是否开启阴影
#define RENDER_FEATURE_AUTO_NORMAL 7		//是否自动设置顶点法向量为三角面的法向量		
#define RENDER_FEATURE_RAY_TRACING 8		// 渲染纹理
#define RENDER_FEATURE_RAY_TRACING_PBR 9	// 渲染纹理

//片段着色器-着色算法
#define RENDER_SHADER_PIXEL_SCANLINE 0b1			//扫描线算法-进行片段着色,更快但不够精准
#define RENDER_SHADER_PIXEL_BOUNDINGBOX 0b10		//边界盒算法-进行片段着色,精准但不快

class Renderer
{
private:
	void draw_pixel(int x, int y, UINT32 color);
	void draw_triangle(vertex_t v1, vertex_t v2, vertex_t v3, Draw_ExtraData extra_data = {});
	void draw_triangle_StandardAlgorithm(const vertex_t& top, const vertex_t& left, const vertex_t& right, const Draw_ExtraData& extra_data);
	void draw_triangle_BresenhamAlgorithm(const vertex_t& top, const vertex_t& left, const vertex_t& right);
	void draw_triangle_BoundingBox(const vertex_t& v1, const vertex_t& v2, const vertex_t& v3);
	void Phong_Shading(vertex_t& v, bool in_shadow);
	color_t Calculate_Lighting(vertex_t& v, const Light* light, bool in_shadow = false);
	int Rendering_RayTracing();
	color_t Ray_Tracing(const ray_t& ray, int depth, float& dis);
	color_t Radiance(const ray_t& ray, int depth);

public:
	transform_t transform;      // 坐标变换器
	const Camera* camera = nullptr;
	float width, height;

	UINT32** frame_buffer = nullptr;		// 像素缓存：frame_buffer[y] 代表第 y行,像素缓存为不同Renderer共用
	float** z_buffer = nullptr;				// 深度缓存：z_buffer[y] 为第 y行指针,深度缓存为每个Renderer独用

	Texture* texture;			// 纹理：每行索引
	int tex_width;              // 纹理宽度
	int tex_height;             // 纹理高度
	float tex_max_u;            // 纹理最大宽度：tex_width - 1
	float tex_max_v;            // 纹理最大高度：tex_height - 1
	int tex_limit_size;

	float min_clip_x = 0;
	float min_clip_y = 0;
	float max_clip_x;
	float max_clip_y;

	UINT32 background;			// 背景颜色
	color_t background_f;
	UINT32 foreground;			// 线框颜色

	int render_state = RENDER_STATE_WIREFRAME;	// 渲染状态
	int render_shader_state = RENDER_SHADER_PIXEL_SCANLINE;	// 片段着色器算法选择

	std::unordered_map<int, bool> features;	//特性

	std::vector<const Light*>lights;
	const Light* current_light;

	std::vector<triangle_t>triangles;
	int raytracing_samples_num = 1;
	int raytracing_max_depth = 5;

	Renderer();

	void init(int width, int height, void* fb);
	void destroy();
	void clear();

	void set_texture(const Texture& tex);
	color_t texture_read(const Texture& tex, float u, float v);
	void add_light(const Light& light);
	int Set_Feature(UINT32 feature, bool turn_on);

	void draw_line(int x1, int y1, int x2, int y2, UINT32 color);
	int	display_primitive(vertex_t v1, vertex_t v2, vertex_t v3);
	int Rendering();
	void transform_update();

	void FXAA(bool on);
};

