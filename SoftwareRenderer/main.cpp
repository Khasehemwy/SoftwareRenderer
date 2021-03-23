#include"Includes.h"


int main()
{
	Window window;
	window.screen_init(800, 600, _T("SoftwareRenderer"));

	Renderer renderer;
	renderer.init(window.screen_width, window.screen_height, window.screen_fb);

	point_t p[5];
	p[1] = { 0,-1,-1,1 };
	p[2] = { 0,1,-1,1 };
	p[3] = { 0,1,1,1 };
	p[4] = { 0,-1,1,1 };
	vertex_t vert[5];
	vert[1].pos = p[1];
	vert[2].pos = p[2];
	vert[3].pos = p[3];
	vert[4].pos = p[4];

	Camera camera;
	camera.init_target_zero({ 3,0,0,1 });

	while (window.screen_exit[0] == 0 && window.screen_keys[VK_ESCAPE] == 0) {
		window.screen_dispatch();
		renderer.clear();

		matrix_t view = camera.set_lookat(camera.camera_pos, camera.target, camera.camera_up);
		renderer.transform.view = view;

		matrix_t model;
		matrix_set_identity(&model);
		//model = matrix_rotate(model, 0, { 1, 1, 1, 1 });
		renderer.transform.model = model;
		renderer.transform_update();
		renderer.display_primitive(vert[1], vert[2], vert[3]);
		renderer.display_primitive(vert[1], vert[3], vert[4]);

		renderer.draw_line(10, 10, 10, 1000, 0x0);

		window.screen_update();
	}

	return 0;
}