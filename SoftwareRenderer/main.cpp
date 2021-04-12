#include"Includes.h"

float cursor_yaw = 0.0f;
float cursor_pitch = 0.0f;
float cursor_last_x = 400, cursor_last_y = 300;
float gl_x_offset = 90.0f;
float gl_y_offset = 0.0f;

void mouse_callback(Camera& camera)
{
	//POINT p;
	//GetCursorPos(&p);
	//float pos_x = p.x, pos_y = p.y;
	//float x_offset = cursor_last_x - pos_x;
	//float y_offset = pos_y - cursor_last_y;
	//cursor_last_x = pos_x;
	//cursor_last_y = pos_y;
	//float sensitivity = 0.05f;
	//x_offset *= sensitivity;
	//y_offset *= sensitivity;

	cursor_yaw += gl_x_offset;
	cursor_pitch += gl_y_offset;
	if (cursor_pitch > 89.0f) { cursor_pitch = 89.0f; }
	if (cursor_pitch < -89.0f) { cursor_pitch = -89.0f; }
	vector_t front;
	front.x = cos(radians(cursor_pitch)) * cos(radians(cursor_yaw));
	front.y = sin(radians(cursor_pitch));
	front.z = cos(radians(cursor_pitch)) * sin(radians(cursor_yaw));
	camera.front = vector_normalize(front);
}

void draw_square(Renderer& renderer, vertex_t lb, vertex_t rb, vertex_t rt, vertex_t lt)
{
	lb.tex = { 0,1 };
	rb.tex = { 1,1 };
	rt.tex = { 1,0 };
	lt.tex = { 0,0 };
	renderer.display_primitive(lb, rb, rt);
	renderer.display_primitive(rt, lt, lb);
}

void draw_box(Renderer& renderer)
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
	vert[0].color = { 0.8,0,0,1 };
	vert[1].color = { 0.8,0,0,1 };
	vert[2].color = { 0.8,0,0,1 };
	vert[3].color = { 0.8,0,0,1 };
	vert[4].color = { 0,0,0.8,1 };
	vert[5].color = { 0,0,0.8,1 };
	vert[6].color = { 0,0,0.8,1 };
	vert[7].color = { 0,0,0.8,1 };
	for (int i = 0; i < 8; i++) {
		vert[i].pos = p[i]*0.5;
	}

	draw_square(renderer, vert[0], vert[1], vert[2], vert[3]);
	draw_square(renderer, vert[1], vert[5], vert[6], vert[2]);
	draw_square(renderer, vert[0], vert[3], vert[7], vert[4]);
	draw_square(renderer, vert[0], vert[4], vert[5], vert[1]);
	draw_square(renderer, vert[3], vert[2], vert[6], vert[7]);
	draw_square(renderer, vert[4], vert[7], vert[6], vert[5]);
}

void draw_light(Renderer& renderer)
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
	vert[0].color = { 1,0.7,0.5,1 };
	vert[1].color = { 1,0.7,0.5,1 };
	vert[2].color = { 1,0.7,0.5,1 };
	vert[3].color = { 1,0.7,0.5,1 };
	vert[4].color = { 1,0.7,0.5,1 };
	vert[5].color = { 1,0.7,0.5,1 };
	vert[6].color = { 1,0.7,0.5,1 };
	vert[7].color = { 1,0.7,0.5,1 };
	for (int i = 0; i < 8; i++) {
		vert[i].pos = p[i];
	}

	draw_square(renderer, vert[3], vert[2], vert[1], vert[0]);
	draw_square(renderer, vert[2], vert[6], vert[5], vert[1]);
	draw_square(renderer, vert[4], vert[7], vert[3], vert[0]);
	draw_square(renderer, vert[1], vert[5], vert[4], vert[0]);
	draw_square(renderer, vert[7], vert[6], vert[2], vert[3]);
	draw_square(renderer, vert[5], vert[6], vert[7], vert[4]);
}

int main()
{
	vector_t cubePositions[] = {
		{0.0f,  0.0f,  0.0f, 1},
		{2.0f,  5.0f, 15.0f, 1},
		{-1.5f, -2.2f, 2.5f, 1},
		{-3.8f, -2.0f, 12.3f, 1},
		{2.4f, -0.4f, 3.5f, 1},
		{-1.7f,  3.0f, 7.5f, 1},
		{1.3f, -2.0f, 2.5f, 1},
		{1.5f,  2.0f, 2.5f, 1},
		{1.5f,  0.2f, 1.5f, 1},
		{-1.3f,  1.0f, 1.5f, 1 }
	};


	Window window;
	window.screen_init(800, 600, _T("SoftwareRenderer - WASD移动,方向键视角,J/L旋转方块."));

	Renderer renderer;
	renderer.init(window.screen_width, window.screen_height, window.screen_fb);
	Renderer renderer_light;
	renderer_light.init(window.screen_width, window.screen_height, window.screen_fb);
	renderer_light.z_buffer = renderer.z_buffer;//因为深度缓存是每个Renderer独用的,但是现在想让它们一起显示.

	Camera camera;
	float posz = -7;
	float posx = 0;
	camera.init_target_zero({ posx,0,posz,1 });
	camera.front = { 0,0,1,1 };

	float angle = 0;
	vector_t rotate_axis = { 1,-0.5,0.5,1 };
	renderer.camera = &camera;
	//renderer.render_state = RENDER_STATE_WIREFRAME;
	//renderer.render_state = RENDER_STATE_COLOR;
	renderer.render_state = RENDER_STATE_TEXTURE;
	//renderer.features[RENDER_FEATURE_BACK_CULLING] = false;
	//renderer.features[RENDER_FEATURE_LIGHT] = false;
	renderer_light.camera = &camera;
	renderer_light.render_state = RENDER_STATE_COLOR;
	renderer_light.features[RENDER_FEATURE_LIGHT] = false;

	vertex_t v1, v2, v3;
	v1.pos = { -1,0,0,1 };
	v2.pos = { 1,0,0,1 };
	v3.pos = { 0,1,0,1 };

	Texture texture;
	texture.init();
	renderer.set_texture(texture.texture, texture.max_size * 4, texture.max_size, texture.max_size);

	Light dir_light;
	dir_light.direction = { -50,-50,-20,1 };
	dir_light.ambient = { 0.3f,0.2f,0.1f,1 };
	dir_light.diffuse = { 0.3f,0.2f,0.1f,1 };
	dir_light.specular = { 0.8f,0.7f,0.6f,1 };
	dir_light.light_state = LIGHT_STATE_DIRECTIONAL;
	renderer.add_light(dir_light);

	Light point_light;
	point_light.pos = { 4,-1,1,1 };
	point_light.ambient = { 0.2f,0.2f,0.2f,1 };
	point_light.diffuse = { 0.9f,0.9f,0.9f,1 };
	point_light.specular = { 1.0f,1.0f,1.0f,1 };
	point_light.light_state = LIGHT_STATE_POINT;
	renderer.add_light(point_light);


	Light spot_light;
	spot_light.light_state = LIGHT_STATE_SPOTLIGHT;
	spot_light.ambient = { 1.0,1.0,1.0,1 };
	spot_light.diffuse = { 0.8,0.8,0.8,1 };
	spot_light.specular = { 0.9,0.9,0.9,1 };
	//手电筒衰减强一些
	spot_light.linear = 0.14;
	spot_light.quadratic = 0.07;
	renderer.add_light(spot_light);

	//时间
	float delta_time = 0.0f;
	float last_frame = 0.0f;
	//鼠标
	//ShowCursor(FALSE);
	//RECT rect;
	//GetWindowRect(*window.screen_handle, &rect);
	//ClipCursor(&rect);
	//SetCapture(*window.screen_handle);

	while (window.screen_exit[0] == 0 && window.screen_keys[VK_ESCAPE] == 0) {
		//时间,使移动速度不受帧率变化
		//float current_frame = time_get();
		//delta_time = current_frame - last_frame;
		//last_frame = current_frame;
		camera.speed = 0.1f;

		//鼠标
		mouse_callback(camera);
		gl_x_offset = 0; gl_y_offset = 0;

		window.screen_dispatch();
		renderer.clear();

		if (window.screen_keys[KEY_J]) angle += 0.01f;
		if (window.screen_keys[KEY_L]) angle -= 0.01f;

		if (window.screen_keys[VK_LEFT]) { gl_x_offset = 0.4f; }
		if (window.screen_keys[VK_RIGHT]) { gl_x_offset = -0.4f; }
		if (window.screen_keys[VK_UP]) { gl_y_offset = 0.4f; }
		if (window.screen_keys[VK_DOWN]) { gl_y_offset = -0.4f; }

		if (window.screen_keys[KEY_A]) {
			camera.pos = camera.pos + (vector_normalize(vector_cross(camera.front, camera.up)) * camera.speed);
		}
		if (window.screen_keys[KEY_D]) {
			camera.pos = camera.pos - (vector_normalize(vector_cross(camera.front, camera.up)) * camera.speed);
		}
		if (window.screen_keys[KEY_W]) {
			camera.pos = camera.pos +
				camera.speed * vector_normalize(vector_cross(camera.up, vector_cross(camera.front, camera.up)));
		}
		if (window.screen_keys[KEY_S]) {
			camera.pos = camera.pos -
				camera.speed * vector_normalize(vector_cross(camera.up, vector_cross(camera.front, camera.up)));
		}

		if (window.screen_keys[KEY_I]) { point_light.pos.z += 0.01f; }
		if (window.screen_keys[KEY_K]) { point_light.pos.z -= 0.01f; }

		//更新摄像机
		camera.target = camera.pos + camera.front;
		matrix_t view = camera.set_lookat(camera.pos, camera.target, camera.up);
		renderer.transform.view = view;

		//设置聚光
		spot_light.pos = camera.pos;
		spot_light.direction = camera.front;

		//画盒子
		matrix_t model;
		for (int i = 0; i < 10; i++) {
			matrix_set_identity(&model);
			model = matrix_translate(model, cubePositions[i]);
			model = model * matrix_rotate_build(angle, rotate_axis);
			renderer.transform.model = model;
			renderer.transform_update();
			draw_box(renderer);
		}

		//画点光源
		renderer_light.transform = renderer.transform;
		matrix_set_identity(&model);
		model = matrix_scale(model, { 0.2,0.2,0.2,1 });
		model = matrix_translate(model, point_light.pos);
		renderer_light.transform.model = model;
		renderer_light.transform_update();
		draw_light(renderer_light);

		//画太阳(定向光)
		matrix_set_identity(&model);
		model = matrix_scale(model, { 10,10,20,1 });
		model = matrix_translate(model, { 50,50,20,1 });
		renderer_light.transform.model = model;
		renderer_light.transform_update();
		draw_light(renderer_light);


		//renderer.display_primitive(vert[1], vert[2], vert[3]);
		//renderer.display_primitive(vert[1], vert[3], vert[4]);
		//renderer.display_primitive(v1, v2, v3);

		renderer.draw_line(10, 10, 20, 10, 0x0);
		renderer.draw_line(10, 10, 10, 20, 0x0);

		window.screen_update();
	}

	return 0;
}