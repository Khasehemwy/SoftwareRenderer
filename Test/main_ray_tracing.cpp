#include"Includes.h"

float cursor_yaw = 0.0f;
float cursor_pitch = 0.0f;
float cursor_last_x = 400, cursor_last_y = 300;
float gl_x_offset = 90.0f;
float gl_y_offset = 0.0f;
float fov = 45.0f;
float angle = 0;
vector_t rotate_axis = { 1,-0.5,0.5,1 };

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
	renderer.display_primitive(rt, rb, lb);
	renderer.display_primitive(lb, lt, rt);
}
void draw_square_out(Renderer& renderer, vertex_t lb, vertex_t rb, vertex_t rt, vertex_t lt)
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
	vert[0].color = color_trans_1f(0xDCCCE4);
	vert[1].color = color_trans_1f(0xDCCCE4);
	vert[2].color = color_trans_1f(0xDCCCE4);
	vert[3].color = color_trans_1f(0xDCCCE4);
	vert[4].color = color_trans_1f(0xDCCCE4);
	vert[5].color = color_trans_1f(0xDCCCE4);
	vert[6].color = color_trans_1f(0xDCCCE4);
	vert[7].color = color_trans_1f(0xDCCCE4);
	for (int i = 0; i < 8; i++) {
		vert[i].pos = p[i] * 0.5;
		vert[i].color.a = 1;
	}

	draw_square_out(renderer, vert[0], vert[1], vert[2], vert[3]);
	draw_square_out(renderer, vert[1], vert[5], vert[6], vert[2]);
	draw_square_out(renderer, vert[0], vert[3], vert[7], vert[4]);
	draw_square_out(renderer, vert[0], vert[4], vert[5], vert[1]);
	draw_square_out(renderer, vert[3], vert[2], vert[6], vert[7]);
	draw_square_out(renderer, vert[4], vert[7], vert[6], vert[5]);
}

void draw_box_no_front(Renderer& renderer)
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
	vert[0].color = color_trans_1f(0xFF969F);
	vert[1].color = color_trans_1f(0x9F95FF);
	vert[2].color = color_trans_1f(0x9F95FF);
	vert[3].color = color_trans_1f(0xFF969F);
	vert[4].color = color_trans_1f(0xFF969F);
	vert[5].color = color_trans_1f(0x9F95FF);
	vert[6].color = color_trans_1f(0x9F95FF);
	vert[7].color = color_trans_1f(0xFF969F);
	for (int i = 0; i < 8; i++) {
		vert[i].pos = p[i] * 0.5;
		vert[i].color.a = 1;
	}

	//draw_square(renderer, vert[0], vert[1], vert[2], vert[3]);
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
	vert[0].color = { 1, 1, 1, 1 };
	vert[1].color = { 1, 1, 1, 1 };
	vert[2].color = { 1, 1, 1, 1 };
	vert[3].color = { 1, 1, 1, 1 };
	vert[4].color = { 1, 1, 1, 1 };
	vert[5].color = { 1, 1, 1, 1 };
	vert[6].color = { 1, 1, 1, 1 };
	vert[7].color = { 1, 1, 1, 1 };
	for (int i = 0; i < 8; i++) {
		vert[i].pos = p[i];
		vert[i].emissivity = color_t(12, 12, 12, 1);
	}

	draw_square_out(renderer, vert[3], vert[2], vert[1], vert[0]);
	draw_square_out(renderer, vert[2], vert[6], vert[5], vert[1]);
	draw_square_out(renderer, vert[4], vert[7], vert[3], vert[0]);
	draw_square_out(renderer, vert[1], vert[5], vert[4], vert[0]);
	draw_square_out(renderer, vert[7], vert[6], vert[2], vert[3]);
	draw_square_out(renderer, vert[5], vert[6], vert[7], vert[4]);
}

void Draw_Scene(Renderer& renderer, Light& point_light)
{
	renderer.transform.view = matrix_lookat(renderer.camera->pos, renderer.camera->target, renderer.camera->up);
	matrix_t model;

	matrix_set_identity(&model);
	model = matrix_translate(model, { 0,0,-5,1 });
	model = matrix_scale(model, { 20,20,20,1 });
	renderer.transform.model = model;
	renderer.transform_update();
	draw_box_no_front(renderer);

	matrix_set_identity(&model);
	model = matrix_translate(model, { 6,-6,0,1 });
	model = matrix_scale(model, { 4,10,4,1 });
	model = model * matrix_rotate_build(radians(45), { 0,1,0,1 });
	renderer.transform.model = model;
	renderer.transform_update();
	draw_box(renderer);

	matrix_set_identity(&model);
	model = matrix_translate(model, { -5,-6,-4,1 });
	model = matrix_scale(model, { 5,5,5,1 });
	model = model * matrix_rotate_build(radians(0), { 0,1,0,1 });
	renderer.transform.model = model;
	renderer.transform_update();
	draw_box(renderer);

	matrix_set_identity(&model);
	model = matrix_scale(model, { 2,0.2,2,1 });
	model = matrix_translate(model, point_light.pos);
	renderer.transform.model = model;
	renderer.transform_update();
	draw_light(renderer);
}

int main()
{
	Window window;
	window.screen_init(300, 300, _T("SoftwareRenderer - WASD移动,方向键视角,J/L旋转方块."));

	Renderer renderer;
	renderer.init(window.screen_width, window.screen_height, window.screen_fb);
	Renderer renderer_light;
	renderer_light.init(window.screen_width, window.screen_height, window.screen_fb);
	renderer_light.z_buffer = renderer.z_buffer;//因为深度缓存是每个Renderer独用的,但是现在想让它们一起显示.
	Renderer renderer_ground;
	renderer_ground.init(window.screen_width, window.screen_height, window.screen_fb);
	renderer_ground.z_buffer = renderer.z_buffer;


	Camera camera;
	float posz = -28;
	float posx = 0;
	camera.init_target_zero({ posx,0,posz,1 });
	camera.front = { 0,0,1,1 };

	renderer.camera = &camera;
	//renderer.render_state = RENDER_STATE_WIREFRAME;
	renderer.render_state = RENDER_STATE_COLOR;
	//renderer.render_state = RENDER_STATE_TEXTURE;
	//renderer.render_shader_state = RENDER_SHADER_PIXEL_BOUNDINGBOX;
	renderer.Set_Feature(RENDER_FEATURE_BACK_CULLING, false);
	renderer.Set_Feature(RENDER_FEATURE_FACK_CULLING, false);
	renderer.Set_Feature(RENDER_FEATURE_CVV_CLIP, false);
	renderer.Set_Feature(RENDER_FEATURE_SHADOW, false);
	//renderer.Set_Feature(RENDER_FEATURE_LIGHT, false);
	//renderer.Set_Feature(RENDER_FEATURE_LIGHT_PHONG, false);
	//renderer.Set_Feature(RENDER_FEATURE_RAY_TRACING, true);
	//renderer.raytracing_max_depth = 16;
	renderer.raytracing_samples_num = 16;

	//renderer-ground
	renderer_ground.camera = &camera;
	renderer_ground.render_state = RENDER_STATE_TEXTURE;
	//renderer_ground.render_state = RENDER_STATE_WIREFRAME;
	renderer_ground.Set_Feature(RENDER_FEATURE_CVV_CLIP, false);
	//renderer_ground.Set_Feature(RENDER_FEATURE_BACK_CULLING, false);
	//renderer_ground.Set_Feature(RENDER_FEATURE_LIGHT_PHONG, false);

	//renderer-light
	renderer_light.camera = &camera;
	renderer_light.render_state = RENDER_STATE_COLOR;
	renderer_light.Set_Feature(RENDER_FEATURE_LIGHT, false);
	renderer_light.Set_Feature(RENDER_FEATURE_SHADOW, false);


	Texture texture(512, 512);
	renderer.Set_Texture("default", &texture);
	renderer_ground.Set_Texture("default", &texture);

	Light dir_light;
	dir_light.pos = { 14,10,10,1 };
	dir_light.direction = { -14,-10,-10,1 };
	//dir_light.direction = { 0,0,0,1 };
	dir_light.ambient = { 0.6f,0.42f,0.3f,1 };
	dir_light.diffuse = { 0.8f,0.56f,0.4f,1 };
	dir_light.specular = { 0.8f,0.56f,0.4f,1 };
	dir_light.light_state = LIGHT_STATE_DIRECTIONAL;
	//renderer.add_light(dir_light);
	//renderer_ground.add_light(dir_light);
	dir_light.Init_ShadowMap(window.screen_width, window.screen_height);

	Light point_light;
	point_light.pos = { 0,7,-9,1 };
	point_light.ambient = { 0.3f,0.3f,0.3f,1 };
	point_light.diffuse = { 0.8f,0.8f,0.8f,1 };
	point_light.specular = { 1.0f,1.0f,1.0f,1 };
	point_light.linear = 0.014;
	point_light.quadratic = 0.0007;
	point_light.light_state = LIGHT_STATE_POINT;
	renderer.add_light(point_light);
	renderer_ground.add_light(point_light);
	point_light.Init_ShadowMap(window.screen_width, window.screen_height);


	Light spot_light;
	spot_light.light_state = LIGHT_STATE_SPOTLIGHT;
	spot_light.ambient = { 1.0,1.0,1.0,1 };
	spot_light.diffuse = { 0.8,0.8,0.8,1 };
	spot_light.specular = { 0.9,0.9,0.9,1 };
	//手电筒衰减强一些
	spot_light.linear = 0.14;
	spot_light.quadratic = 0.07;
	//renderer.add_light(spot_light);
	//renderer_ground.add_light(spot_light);
	spot_light.Init_ShadowMap(window.screen_width, window.screen_height);

	//阴影
	Renderer renderer_shadow;
	renderer_shadow.init(window.screen_width, window.screen_height, window.screen_fb);
	renderer_shadow.render_state = RENDER_STATE_DEEP;
	renderer_shadow.Set_Feature(RENDER_FEATURE_SHADOW, false);
	renderer_shadow.Set_Feature(RENDER_FEATURE_CVV_CLIP, false);
	renderer_shadow.Set_Feature(RENDER_FEATURE_LIGHT, false);
	renderer_shadow.Set_Feature(RENDER_FEATURE_LIGHT_PHONG, false);
	renderer_shadow.Set_Feature(RENDER_FEATURE_BACK_CULLING, false);
	renderer_shadow.Set_Feature(RENDER_FEATURE_FACK_CULLING, true);
	Camera shadow_camera;
	renderer_shadow.camera = &shadow_camera;
	shadow_camera.pos = dir_light.pos;
	shadow_camera.init_target_zero(dir_light.pos);
	shadow_camera.front = dir_light.direction;
	shadow_camera.target = shadow_camera.pos + shadow_camera.front;
	renderer_shadow.transform.view = matrix_lookat(shadow_camera.pos, shadow_camera.target, shadow_camera.up);
	float shadow_ortho = 0.4f;
	renderer_shadow.transform.projection = matrix_ortho(-shadow_ortho, shadow_ortho, -shadow_ortho, shadow_ortho, 1.0f, 100.0f);

	//设置主renderer
	float aspect = (float)renderer.width / ((float)renderer.height);
	//camera = shadow_camera;
	renderer.current_light = &dir_light;
	renderer_ground.current_light = &dir_light;

	//时间
	float delta_time = 0.0f;
	float last_frame = 0.0f;
	//鼠标
	//ShowCursor(FALSE);
	//RECT rect;
	//GetWindowRect(*window.screen_handle, &rect);
	//ClipCursor(&rect);
	//SetCapture(*window.screen_handle);

	//FPS
	FPS* fps = new FPS();

	int operation = 0;
	while (window.screen_exit[0] == 0 && window.screen_keys[VK_ESCAPE] == 0) {
		std::thread t_operation([&]() {std::cin >> operation; });
		t_operation.join();
		if (operation == 0) {
			renderer.Set_Feature(RENDER_FEATURE_RAY_TRACING, false);
			renderer_ground.Set_Feature(RENDER_FEATURE_RAY_TRACING, false);
			renderer.Set_Feature(RENDER_FEATURE_RAY_TRACING_PBR, false);
			renderer_ground.Set_Feature(RENDER_FEATURE_RAY_TRACING_PBR, false);
		}
		else if (operation == 1) {
			renderer.Set_Feature(RENDER_FEATURE_RAY_TRACING, true);
			renderer_ground.Set_Feature(RENDER_FEATURE_RAY_TRACING, true);
			renderer.Set_Feature(RENDER_FEATURE_RAY_TRACING_PBR, true);
			renderer_ground.Set_Feature(RENDER_FEATURE_RAY_TRACING_PBR, true);
		}

		//时间,使移动速度不受帧率变化
		//float current_frame = time_get();
		//delta_time = current_frame - last_frame;
		//last_frame = current_frame;
		camera.speed = 0.2f;

		//鼠标
		mouse_callback(camera);
		gl_x_offset = 0; gl_y_offset = 0;

		fps->Print_FPS();

		window.screen_dispatch();
		renderer.clear();

		if (window.screen_keys[KEY_J]) point_light.pos.x -= 0.1f;
		if (window.screen_keys[KEY_L]) point_light.pos.x += 0.1f;

		if (window.screen_keys[VK_LEFT]) { gl_x_offset = 0.5f; }
		if (window.screen_keys[VK_RIGHT]) { gl_x_offset = -0.5f; }
		if (window.screen_keys[VK_UP]) { gl_y_offset = 0.5f; }
		if (window.screen_keys[VK_DOWN]) { gl_y_offset = -0.5f; }

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

		if (window.screen_keys[KEY_I]) { point_light.pos.z += 0.1f; }
		if (window.screen_keys[KEY_K]) { point_light.pos.z -= 0.1f; }
		if (window.screen_keys[KEY_U]) { point_light.pos.y += 0.1f; }
		if (window.screen_keys[KEY_O]) { point_light.pos.y -= 0.1f; }

		if (window.screen_keys[KEY_Y]) { fov -= 0.04f; }
		if (window.screen_keys[KEY_H]) { fov += 0.04f; }
		fov = CMID(fov, 1.0f, 60.0f);
		camera.fov = fov;
		//camera.front = { 0.680726,-0.0610485,-0.72999,1 };
		//std::cout << camera.front.x << "," << camera.front.y << "," << camera.front.z << "\n";

		matrix_t model;

		//*************阴影*************//
		//只渲染dir_light的阴影
		//Draw_Scene(renderer_shadow, renderer_shadow);
		//dir_light.Set_ShadowMap(renderer_shadow);
		//renderer_shadow.clear();

		//************正常场景************//
		//更新摄像机
		camera.target = camera.pos + camera.front;
		matrix_t view = matrix_lookat(renderer.camera->pos, renderer.camera->target, renderer.camera->up);
		renderer.transform.view = view;

		matrix_set_perspective(&renderer.transform.projection, fov, aspect, 0.1f, 100.0f);

		//设置聚光
		spot_light.pos = camera.pos;
		spot_light.direction = camera.front;

		//画盒子 & 地面
		Draw_Scene(renderer, point_light);

		////画太阳(定向光)
		//matrix_set_identity(&model);
		//model = matrix_scale(model, { 2,2,5,1 });
		//model = matrix_translate(model, { dir_light.pos.x, dir_light.pos.y, dir_light.pos.z, 1 });
		//renderer_light.transform.model = model;
		//renderer_light.transform_update();
		//draw_light(renderer_light);



		//renderer.display_primitive(vert[1], vert[2], vert[3]);
		//renderer.display_primitive(vert[1], vert[3], vert[4]);
		//renderer.display_primitive(v1, v2, v3);

		//画点光源
		//renderer.transform = renderer.transform;
		//matrix_set_identity(&model);
		//model = matrix_scale(model, { 2,0.2,2,1 });
		//model = matrix_translate(model, point_light.pos);
		//renderer.transform.model = model;
		//renderer.transform_update();
		//draw_light(renderer);

		renderer.Rendering();


		renderer.draw_line(10, 10, 20, 10, 0x0);
		renderer.draw_line(10, 10, 10, 20, 0x0);

		renderer.triangles.clear();

		window.screen_update();

		//renderer.FXAA(true);
	}

	return 0;
}