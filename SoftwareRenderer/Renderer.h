#pragma once
#include"Includes.h"

#define RENDER_STATE_WIREFRAME      1		// ��Ⱦ�߿�

class Renderer
{
public:
	transform_t transform;      // ����任��
	float width, height;

	UINT32** frame_buffer;		// ���ػ��棺framebuffer[y] ����� y��
	float** z_buffer;			// ��Ȼ��棺zbuffer[y] Ϊ�� y��ָ��

	UINT32 background;			// ������ɫ
	UINT32 foreground;			// �߿���ɫ

	int render_state = RENDER_STATE_WIREFRAME;           // ��Ⱦ״̬

	void init(int width, int height, void* fb);
	void destroy();
	void clear();

	void draw_pixel(int x, int y, UINT32 color);
	void draw_line(int x1, int y1, int x2, int y2, UINT32 color);
	int display_primitive(const vertex_t& v1, const vertex_t& v2, const vertex_t& v3);
	void transform_update();
};

