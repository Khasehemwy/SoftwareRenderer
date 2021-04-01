#include"Includes.h"


void draw_square(Renderer& renderer, vertex_t lb, vertex_t rb, vertex_t rt, vertex_t lt)
{
	lb.tex = { 0,1 };
	rb.tex = { 1,1 };
	rt.tex = { 1,0 };
	lt.tex = { 0,0 };
	renderer.display_primitive(lb, rb, rt);
	renderer.display_primitive(rt, lt, lb);
}

void draw_box(Renderer& renderer, Light& light)
{
	point_t p[8];
	p[0] = { -1,-1,-1,1 };
	p[1] = { 1,-1,-1,1 };
	p[2] = { 1,1,-1,1 };
	p[3] = { -1,1,-1,1 };
	p[4] = { -1,-1,1,1 };
	p[5] = { 1,-1,1,1 };
	p[6] = { 1,1,1,1 };
	p[7] = { -1,1,1,1 };

	vertex_t vert[8];
	vert[0].color = { 1,0,0,1 };
	vert[1].color = { 1,0,0,1 };
	vert[2].color = { 1,0,0,1 };
	vert[3].color = { 1,0,0,1 };
	vert[4].color = { 0.2,1,1,1 };
	vert[5].color = { 0,0,1,1 };
	vert[6].color = { 0,0,1,1 };
	vert[7].color = { 0,0,1,1 };
	std::vector<vertex_t>vs;
	for (int i = 0; i < 8; i++) {
		vert[i].pos = p[i];
		vs.push_back(vert[i]);
	}
	////light.vertexs = &vs;
	//light.set_vertex();

	//vertex_t light_v[3];
	//for (int i = 0; i < 3; i++) { light_v[i].pos = light.pos; light_v[i].color = light.color; }
	//light_v[0].pos.x -= 0.2;
	//light_v[1].pos.x += 0.2;
	//light_v[2].pos.y += 0.2;

	draw_square(renderer, vert[0], vert[1], vert[2], vert[3]);
	draw_square(renderer, vert[1], vert[5], vert[6], vert[2]);
	draw_square(renderer, vert[0], vert[3], vert[7], vert[4]);
	draw_square(renderer, vert[0], vert[4], vert[5], vert[1]);
	draw_square(renderer, vert[3], vert[2], vert[6], vert[7]);
	draw_square(renderer, vert[4], vert[7], vert[6], vert[5]);
	//renderer.display_primitive(light_v[0], light_v[1], light_v[2]);
}

int main()
{
	Window window;
	window.screen_init(800, 600, _T("SoftwareRenderer"));

	Renderer renderer;
	renderer.init(window.screen_width, window.screen_height, window.screen_fb);

	Camera camera;
	float pos = -5;
	camera.init_target_zero({ 0,0,pos,1 });

	float angle = 0;
	vector_t rotate_axis = { 1,-0.5,0.5,1 };
	renderer.camera = &camera;
	//renderer.render_state = RENDER_STATE_WIREFRAME;
	//renderer.render_state = RENDER_STATE_COLOR;
	renderer.render_state = RENDER_STATE_TEXTURE;
	//renderer.features[RENDER_FEATURE_BACK_CULLING] = false;

	vertex_t v1, v2, v3;
	v1.pos = { -1,0,0,1 };
	v2.pos = { 1,0,0,1 };
	v3.pos = { 0,1,0,1 };

	Texture texture;
	texture.init();
	renderer.set_texture(texture.texture, texture.max_size * 4, texture.max_size, texture.max_size);

	Light light;
	light.pos = { -1,0,0,1 };
	light.color = { 1,1,1,1 };

	while (window.screen_exit[0] == 0 && window.screen_keys[VK_ESCAPE] == 0) {
		window.screen_dispatch();
		renderer.clear();

		matrix_t view = camera.set_lookat(camera.camera_pos, camera.target, camera.camera_up);
		renderer.transform.view = view;
		matrix_t model;
		matrix_set_identity(&model);
		model = model * matrix_rotate_build(angle, rotate_axis);
		renderer.transform.model = model;
		
		renderer.transform_update();

		if (window.screen_keys[VK_UP]) pos += 0.01f;
		if (window.screen_keys[VK_DOWN]) pos -= 0.01f;
		if (window.screen_keys[VK_LEFT])angle -= 0.01f;
		if (window.screen_keys[VK_RIGHT])angle += 0.01f;
		if (window.screen_keys[0x41]) { light.pos.x -= 0.01f; }
		if (window.screen_keys[0x44]) { light.pos.x += 0.01f; }
		if (window.screen_keys[0x57]) { light.pos.y -= 0.01f; }
		if (window.screen_keys[0x53]) { light.pos.y += 0.01f; }
		camera.camera_pos.z = pos;

		//renderer.display_primitive(vert[1], vert[2], vert[3]);
		//renderer.display_primitive(vert[1], vert[3], vert[4]);

		draw_box(renderer,light);
		//renderer.display_primitive(v1, v2, v3);

		renderer.draw_line(10, 10, 20, 10, 0x0);
		renderer.draw_line(10, 10, 10, 20, 0x0);

		window.screen_update();
	}

	return 0;
}