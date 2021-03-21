#include"Includes.h"


int main()
{
	Window window;
	window.screen_init(800, 600, _T("SoftwareRenderer"));

	Renderer renderer;
	renderer.init(window.screen_width, window.screen_height, window.screen_fb);

	point_t p1, p2, p3;
	p1 = { -1,-1,0,1 };
	p2 = { 1,-1,0,1 };
	p3 = { 0,1,0,1 };
	vertex_t vert1, vert2, vert3;
	vert1.pos = p1;
	vert2.pos = p2;
	vert3.pos = p3;

	while (window.screen_exit[0] == 0 && window.screen_keys[VK_ESCAPE] == 0) {
		window.screen_dispatch();
		renderer.clear();

		renderer.draw_line(10, 10, 10, 1000, 0x0);

		window.screen_update();
	}

	return 0;
}