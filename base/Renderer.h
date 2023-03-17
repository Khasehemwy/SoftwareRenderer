#pragma once
#include"Includes.h"


//渲染模式
#define RENDER_STATE_WIREFRAME 1		// 渲染线框
#define RENDER_STATE_COLOR 2			// 渲染颜色
#define RENDER_STATE_TEXTURE 3			// 渲染纹理
#define RENDER_STATE_DEEP 4				// 渲染深度

//特性
#define RENDER_FEATURE_BACK_CULLING 1		//背面剔除
#define RENDER_FEATURE_FACK_CULLING 2		//正面剔除
#define RENDER_FEATURE_LIGHT 3				//是否开启光照(默认高洛德着色)
#define RENDER_FEATURE_LIGHT_PHONG 4		//是否开启光照的冯氏着色
#define RENDER_FEATURE_CVV_CLIP 5			//是否开启cvv裁剪
#define RENDER_FEATURE_SHADOW 6				//是否开启阴影
#define RENDER_FEATURE_AUTO_NORMAL 7		//是否自动设置顶点法向量为三角面的法向量		
#define RENDER_FEATURE_RAY_TRACING 8		//是否使用光追渲染
#define RENDER_FEATURE_RAY_TRACING_PBR 9	//是否使用基于PBR的光追
#define RENDER_FEATURE_DEPTH_WRITE 10		//深度写入
#define RENDER_FEATURE_DEPTH_TEST 11		//深度测试

//深度测试模式
#define RENDER_DEPTH_TEST_ALWAYS 0
#define RENDER_DEPTH_TEST_NEVER 1
#define RENDER_DEPTH_TEST_LESS 2
#define RENDER_DEPTH_TEST_EQUAL 3
#define RENDER_DEPTH_TEST_GREATER 4

//片段着色器-着色算法
#define RENDER_SHADER_PIXEL_SCANLINE 0b1			//扫描线算法-进行片段着色,更快但不够精准
#define RENDER_SHADER_PIXEL_EDGEEQUATION 0b10		//边界盒算法-进行片段着色,精准但不快

class Renderer
{
private:
	void draw_pixel(int x, int y, UINT32 color);
	void draw_triangle(vertex_t v1, vertex_t v2, vertex_t v3);
	void draw_triangle_StandardAlgorithm(const vertex_t& top, const vertex_t& left, const vertex_t& right);
	void draw_triangle_BresenhamAlgorithm(const vertex_t& top, const vertex_t& left, const vertex_t& right);
	void draw_triangle_EdgeEquation(const vertex_t& v1, const vertex_t& v2, const vertex_t& v3);
	void Phong_Shading(vertex_t& v, bool in_shadow);
	color_t Calculate_Lighting(vertex_t& v, const Light* light, bool in_shadow = false);

	void Draw_Fragment(const vertex_t * v1, const vertex_t * v2, const vertex_t * v3, barycentric_t bary, int x, int y);

	int Rendering_RayTracing();
	color_t Ray_Tracing(const ray_t& ray, int depth, float& dis);
	color_t Radiance(const ray_t& ray, int depth);


	virtual void VS(vertex_t* v1, vertex_t* v2, vertex_t* v3);

	virtual vertex_t Split_Triangle(
		const vertex_t* v1, const vertex_t* v2, const vertex_t* v3, float t);

	virtual color_t PS_Interpolation(vertex_t* v1, vertex_t* v2, vertex_t* v3, barycentric_t bary);
	virtual color_t PS(vertex_t* v);

public:
	transform_t transform;      // 坐标变换器
	const Camera* camera = nullptr;
	float width, height;

	UINT32** frame_buffer = nullptr;		// 像素缓存：frame_buffer[y] 代表第 y行,像素缓存为不同Renderer共用
	float** z_buffer = nullptr;				// 深度缓存：z_buffer[y] 为第 y行指针,深度缓存为每个Renderer独用

	std::unordered_map<std::string, const Texture*> textures;
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
	int depth_test_state = RENDER_DEPTH_TEST_LESS;

	std::vector<const Light*>lights;
	const Light* current_light;

	std::vector<triangle_t>triangles;
	int raytracing_samples_num = 1;
	int raytracing_max_depth = 5;

	Renderer();

	void init(int width, int height, void* fb);
	void destroy();
	void clear();

	void Set_Texture(std::string name, const Texture* tex);

	void add_light(const Light& light);

	int Set_Feature(UINT32 feature, bool turn_on);

	void draw_line(int x1, int y1, int x2, int y2, UINT32 color);
	int	display_primitive(vertex_t v1, vertex_t v2, vertex_t v3);
	int Rendering();
	void transform_update();

	void FXAA(bool on);
};

