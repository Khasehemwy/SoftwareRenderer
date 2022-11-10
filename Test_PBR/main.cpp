#include"Includes.h"
#include"shader.h"

float cursor_yaw = 0.0f;
float cursor_pitch = 0.0f;
float cursor_last_x = 400, cursor_last_y = 300;
float gl_x_offset = 90.0f;
float gl_y_offset = 0.0f;
float g_fov = 45.0f;



void mouse_callback(Camera& camera)
{
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
	vert[0].color = { 20.0f,0.6f,0.6f,1 };
	vert[1].color = { 20.0f,0.6f,0.6f,1 };
	vert[2].color = { 20.0f,0.6f,0.6f,1 };
	vert[3].color = { 20.0f,0.6f,0.6f,1 };
	vert[4].color = { 20.0f,0.6f,0.6f,1 };
	vert[5].color = { 20.0f,0.6f,0.6f,1 };
	vert[6].color = { 20.0f,0.6f,0.6f,1 };
	vert[7].color = { 20.0f,0.6f,0.6f,1 };
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
	Window window;
	window.screen_init(800, 600, _T("SoftwareRenderer - model."));

	Renderer_PBR renderer;
	renderer.init(window.screen_width, window.screen_height, window.screen_fb);
	Renderer renderer_light;
	renderer_light.init(window.screen_width, window.screen_height, window.screen_fb);
	renderer_light.z_buffer = renderer.z_buffer;//因为深度缓存是每个Renderer独用的,但是现在想让它们一起显示.

	float aspect = (float)renderer.width / ((float)renderer.height);

	Camera camera;
	float posz = -3;
	float posx = 0;
	camera.init_target_zero({ posx,0,posz,1 });
	camera.front = { 0,0,1,1 };

	float angle = 0;
	vector_t rotate_axis = { 1,-0.5,0.5,1 };
	renderer.camera = &camera;
	//renderer.render_state = RENDER_STATE_WIREFRAME;
	renderer.render_state = RENDER_STATE_COLOR;
	renderer.features[RENDER_FEATURE_LIGHT] = false;
	renderer.features[RENDER_FEATURE_AUTO_NORMAL] = true;
	renderer.features[RENDER_FEATURE_CVV_CLIP] = false;
	//renderer.render_state = RENDER_STATE_TEXTURE;
	//renderer.features[RENDER_FEATURE_BACK_CULLING] = false;
	renderer_light.camera = &camera;
	renderer_light.render_state = RENDER_STATE_COLOR;
	renderer_light.features[RENDER_FEATURE_LIGHT] = false;

	Model models("../resources/sphere/scene.gltf");
	//Model models("../resources/room/OBJ/room.obj");

	Texture tex_wooden_diffuce("../resources/patterned_wooden_wall_panels_48_05_2K/diffuse.jpg");
	Texture tex_wooden_roughness("../resources/patterned_wooden_wall_panels_48_05_2K/roughness.jpg");
	Texture tex_wooden_normal("../resources/patterned_wooden_wall_panels_48_05_2K/normal.jpg");
	//Texture tex_wooden_diffuce("../resources/sphere/textures/Material_baseColor.png");
	//Texture tex_wooden_roughness("../resources/sphere/textures/Material_metallicRoughness.png");
	//Texture tex_wooden_normal("../resources/sphere/textures/Material_normal.png");
	Texture tex_wooden_ao("../resources/patterned_wooden_wall_panels_48_05_2K/ao.jpg");
	Texture tex_wooden_glossiness("../resources/patterned_wooden_wall_panels_48_05_2K/glossiness.jpg");
	Texture tex_wooden_metallic("../resources/patterned_wooden_wall_panels_48_05_2K/metallic.jpg");
	Texture tex_wooden_reflection("../resources/patterned_wooden_wall_panels_48_05_2K/reflection.jpg");
	renderer.Add_Texture("diffuce", &tex_wooden_diffuce);
	renderer.Add_Texture("ao", &tex_wooden_ao);
	renderer.Add_Texture("glossiness", &tex_wooden_glossiness);
	renderer.Add_Texture("metallic", &tex_wooden_metallic);
	renderer.Add_Texture("normal", &tex_wooden_normal);
	renderer.Add_Texture("reflection", &tex_wooden_reflection);
	renderer.Add_Texture("roughness", &tex_wooden_roughness);

	//光源
	Light point_light;
	point_light.pos = { 1,-1,-4,1 };
	point_light.radiance = { 5.0f,4.8f,4.8f,1 };
	point_light.light_state = LIGHT_STATE_POINT;

	renderer.add_light(point_light);

	//时间
	float delta_time = 0.0f;
	float last_frame = 0.0f;

	FPS* fps = new FPS();

	while (window.screen_exit[0] == 0 && window.screen_keys[VK_ESCAPE] == 0) {
		fps->Print_FPS();

		//时间,使移动速度不受帧率变化
		//不好用,暂时舍弃
		float current_frame = time_get();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;
		camera.speed = 0.1f;

		//鼠标
		mouse_callback(camera);
		gl_x_offset = 0; gl_y_offset = 0;

		window.screen_dispatch();
		renderer.clear();

		if (window.screen_keys[KEY_J]) angle += 0.01f;
		if (window.screen_keys[KEY_L]) angle -= 0.01f;

		float head_speed = 8;
		if (window.screen_keys[VK_LEFT]) { gl_x_offset = head_speed * camera.speed; }
		if (window.screen_keys[VK_RIGHT]) { gl_x_offset = -head_speed * camera.speed; }
		if (window.screen_keys[VK_UP]) { gl_y_offset = head_speed * camera.speed; }
		if (window.screen_keys[VK_DOWN]) { gl_y_offset = -head_speed * camera.speed; }

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
		if (window.screen_keys[KEY_Q]) {
			camera.pos.y = camera.pos.y + camera.speed;
		}
		if (window.screen_keys[KEY_E]) {
			camera.pos.y = camera.pos.y - camera.speed;
		}

		if (window.screen_keys[KEY_Y]) { g_fov -= 0.04f; }
		if (window.screen_keys[KEY_H]) { g_fov += 0.04f; }
		g_fov = CMID(g_fov, 1.0f, 60.0f);

		//更新摄像机
		camera.target = camera.pos + camera.front;
		matrix_t view = camera.set_lookat(camera.pos, camera.target, camera.up);
		renderer.transform.view = view;

		matrix_set_perspective(&renderer.transform.projection, g_fov, aspect, 0.1f, 100.0f);

		renderer.view_pos = camera.pos;

		matrix_t model;
		matrix_set_identity(&model);
		//model = matrix_scale(model, { 0.02f,0.02f,0.02f,1 });
		//model = matrix_scale(model, { 0.1f,0.1f,0.1f,1 });
		model = model * matrix_rotate_build(radians(90), { 1.0f,0.0f,0.0f,1 });
		//model = model * matrix_rotate_build(radians(30), { 0.0f,1.0f,0.0f,1 });
		//model = matrix_translate(model, { 0.0f,-1.5f,0.0f,1 });
		renderer.transform.model = model;
		renderer.transform_update();
		models.draw(renderer);

		//画光源
		renderer_light.transform = renderer.transform;
		matrix_set_identity(&model);
		model = matrix_scale(model, { 0.2,0.2,0.2,1 });
		model = matrix_translate(model, point_light.pos);
		renderer_light.transform.model = model;
		renderer_light.transform_update();
		draw_light(renderer_light);

		renderer.draw_line(10, 10, 20, 10, 0x0);
		renderer.draw_line(10, 10, 10, 20, 0x0);

		renderer.Rendering();

		window.screen_update();
	}

	return 0;
}


