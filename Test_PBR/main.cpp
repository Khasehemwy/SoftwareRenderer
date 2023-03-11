#include"Includes.h"
#include"shader.h"

#define DRAW_SKYBOX
#define DRAW_MODEL

float cursor_yaw = 0.0f;
float cursor_pitch = 0.0f;
float cursor_last_x = 400, cursor_last_y = 300;
float gl_x_offset = 90.0f;
float gl_y_offset = 0.0f;
float g_fov = radians(45.0);



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

void draw_square(Renderer& renderer, vertex_t lb, vertex_t rb, vertex_t rt, vertex_t lt, vector_t normal = { 0,0,0,0 })
{
	lb.tex = { 0,1 };
	rb.tex = { 1,1 };
	rt.tex = { 1,0 };
	lt.tex = { 0,0 };
	lb.normal = rb.normal = rt.normal = lt.normal = normal;
	renderer.display_primitive(lb, rb, rt);
	renderer.display_primitive(rt, lt, lb);
}

void draw_light(Renderer& renderer)
{
	point_t p[8];
	p[0] = { -0.5,-0.5,-0.5,1 };
	p[1] = { 0.5,-0.5,-0.5,1 };
	p[2] = { 0.5,0.5,-0.5,1 };
	p[3] = { -0.5,0.5,-0.5,1 };
	p[4] = { -0.5,-0.5,0.5,1 };
	p[5] = { 0.5,-0.5,0.5,1 };
	p[6] = { 0.5,0.5,0.5,1 };
	p[7] = { -0.5,0.5,0.5,1 };

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

	draw_square(renderer, vert[0], vert[1], vert[2], vert[3]);
	draw_square(renderer, vert[1], vert[5], vert[6], vert[2]);
	draw_square(renderer, vert[0], vert[3], vert[7], vert[4]);
	draw_square(renderer, vert[0], vert[4], vert[5], vert[1]);
	draw_square(renderer, vert[3], vert[2], vert[6], vert[7]);
	draw_square(renderer, vert[4], vert[7], vert[6], vert[5]);
}

void draw_skybox(Renderer& renderer)
{
	point_t p[8];
	p[0] = { -0.5,-0.5,-0.5,1 };
	p[1] = { 0.5,-0.5,-0.5,1 };
	p[2] = { 0.5,0.5,-0.5,1 };
	p[3] = { -0.5,0.5,-0.5,1 };
	p[4] = { -0.5,-0.5,0.5,1 };
	p[5] = { 0.5,-0.5,0.5,1 };
	p[6] = { 0.5,0.5,0.5,1 };
	p[7] = { -0.5,0.5,0.5,1 };

	vertex_t vert[8];
	for (int i = 0; i < 8; i++) {
		vert[i].pos = p[i];
	}

	draw_square(renderer, vert[0], vert[1], vert[2], vert[3]);
	draw_square(renderer, vert[1], vert[5], vert[6], vert[2]);
	draw_square(renderer, vert[0], vert[3], vert[7], vert[4]);
	draw_square(renderer, vert[0], vert[4], vert[5], vert[1]);
	draw_square(renderer, vert[3], vert[2], vert[6], vert[7]);
	draw_square(renderer, vert[4], vert[7], vert[6], vert[5]);
}

int main()
{
	Window window;
	window.screen_init(600, 600, _T("SoftwareRenderer - PBR"));

	Renderer_PBR renderer;
	renderer.init(window.screen_width, window.screen_height, window.screen_fb);
	Renderer renderer_light;
	renderer_light.init(window.screen_width, window.screen_height, window.screen_fb);
	renderer_light.z_buffer = renderer.z_buffer;//��Ϊ��Ȼ�����ÿ��Renderer���õ�,����������������һ����ʾ.

	Renderer_Skybox renderer_sky;
	renderer_sky.init(window.screen_width, window.screen_height, window.screen_fb);

	float aspect = (float)renderer.width / ((float)renderer.height);

	Camera camera;
	camera.fov = g_fov;
	float posz = -4;
	float posx = 0;
	camera.init_target_zero({ posx,0,posz,1 });
	camera.front = { 0,0,1,1 };

	float angle = 0;
	vector_t rotate_axis = { 1,-0.5,0.5,1 };
	renderer.camera = &camera;
	//renderer.render_state = RENDER_STATE_WIREFRAME;
	renderer.render_state = RENDER_STATE_TEXTURE;
	renderer.features[RENDER_FEATURE_LIGHT] = false;
	renderer.features[RENDER_FEATURE_AUTO_NORMAL] = true;
	renderer.features[RENDER_FEATURE_CVV_CLIP] = false;
	//renderer.render_state = RENDER_STATE_TEXTURE;
	//renderer.features[RENDER_FEATURE_BACK_CULLING] = false;

	renderer_light.camera = &camera;
	renderer_light.render_state = RENDER_STATE_COLOR;
	renderer_light.features[RENDER_FEATURE_LIGHT] = false;

	renderer_sky.camera = &camera;
	renderer_sky.render_state = RENDER_STATE_TEXTURE;
	renderer_sky.features[RENDER_FEATURE_LIGHT] = false;
	renderer_sky.features[RENDER_FEATURE_CVV_CLIP] = false;
	renderer_sky.features[RENDER_FEATURE_BACK_CULLING] = false;
	renderer_sky.features[RENDER_FEATURE_FACK_CULLING] = false;
	renderer_sky.features[RENDER_FEATURE_DEPTH_TEST] = false;
	renderer_sky.features[RENDER_FEATURE_DEPTH_WRITE] = false;
	//renderer_sky.features[RENDER_FEATURE_AUTO_NORMAL] = false;

	Model models("../resources/sphere/scene.gltf");
	//Model models("../resources/room/OBJ/room.obj");

	std::string texture_name = "cgaxis_stained_patterned_metal_26_98_2K";
	renderer.enable_gamma = true;

	Texture tex_wooden_diffuce("../resources/" + texture_name + "/diffuse.jpg");
	Texture tex_wooden_roughness("../resources/" + texture_name + "/roughness.jpg");
	Texture tex_wooden_normal("../resources/" + texture_name + "/normal.jpg");
	//Texture tex_wooden_diffuce("../resources/sphere/textures/Material_baseColor.png");
	//Texture tex_wooden_roughness("../resources/sphere/textures/Material_metallicRoughness.png");
	//Texture tex_wooden_normal("../resources/sphere/textures/Material_normal.png");
	Texture tex_wooden_ao("../resources/" + texture_name + "/ao.jpg");
	Texture tex_wooden_height("../resources/" + texture_name + "/height.jpg");
	Texture tex_wooden_glossiness("../resources/" + texture_name + "/glossiness.jpg");
	Texture tex_wooden_metallic("../resources/" + texture_name + "/metallic.jpg");
	Texture tex_wooden_reflection("../resources/" + texture_name + "/reflection.jpg");
	renderer.Add_Texture("diffuse", &tex_wooden_diffuce);
	renderer.Add_Texture("ao", &tex_wooden_ao);
	renderer.Add_Texture("height", &tex_wooden_height);
	renderer.Add_Texture("glossiness", &tex_wooden_glossiness);
	renderer.Add_Texture("metallic", &tex_wooden_metallic);
	renderer.Add_Texture("normal", &tex_wooden_normal);
	renderer.Add_Texture("reflection", &tex_wooden_reflection);
	renderer.Add_Texture("roughness", &tex_wooden_roughness);

	std::array<std::filesystem::path, 6>texture_skybox_path;
	std::array<std::filesystem::path, 6>texture_irradiance_path;
	std::array<std::array<std::filesystem::path, 6>, 5>texture_environment_path;
	std::string texture_skybox_name = "sky2";

	texture_skybox_path[0] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/main/px.png";
	texture_skybox_path[1] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/main/nx.png";
	texture_skybox_path[2] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/main/ny.png";
	texture_skybox_path[3] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/main/py.png";
	texture_skybox_path[4] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/main/pz.png";
	texture_skybox_path[5] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/main/nz.png";

	texture_irradiance_path[0] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/irradiance/px.hdr";
	texture_irradiance_path[1] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/irradiance/nx.hdr";
	texture_irradiance_path[2] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/irradiance/ny.hdr";
	texture_irradiance_path[3] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/irradiance/py.hdr";
	texture_irradiance_path[4] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/irradiance/pz.hdr";
	texture_irradiance_path[5] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/irradiance/nz.hdr";

	for (int i = 0; i < 5; i++) {
		std::string mipLevelStr = "";
		if (i > 0) { mipLevelStr += std::to_string(i); }
		texture_environment_path[i][0] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/prefilter/px" + mipLevelStr + ".hdr";
		texture_environment_path[i][1] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/prefilter/nx" + mipLevelStr + ".hdr";
		texture_environment_path[i][2] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/prefilter/ny" + mipLevelStr + ".hdr";
		texture_environment_path[i][3] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/prefilter/py" + mipLevelStr + ".hdr";
		texture_environment_path[i][4] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/prefilter/pz" + mipLevelStr + ".hdr";
		texture_environment_path[i][5] = (std::string)"../resources/" + "skybox/" + texture_skybox_name + "/prefilter/nz" + mipLevelStr + ".hdr";

		renderer.cube_textures["environment" + std::to_string(i)] = new Texture_Cube(texture_environment_path[i]);
	}

	Texture_Cube tex_sky_box(texture_skybox_path);
	Texture_Cube tex_sky_irradiance(texture_irradiance_path);
	Texture tex_brdf_lut("../resources/skybox/BRDF_LUT.png");

	renderer_sky.texture_cube = &tex_sky_box;
	renderer.cube_textures["irradiance"] = &tex_sky_irradiance;
	renderer.Add_Texture("BRDF_LUT", &tex_brdf_lut);


	//��Դ
	Light point_light_right;
	point_light_right.pos = { 3,3,-3,1 };
	point_light_right.radiance = { 500.0f,500.0f,500.0f,1 };
	point_light_right.light_state = LIGHT_STATE_POINT;
	renderer.add_light(point_light_right);

	Light point_light_left;
	point_light_left.pos = { -5,2,-0.5,1 };
	point_light_left.radiance = { 100.0f,100.0f,100.0f,1 };
	point_light_left.light_state = LIGHT_STATE_POINT;
	renderer.add_light(point_light_left);
	
	//ʱ��
	float delta_time = 0.0f;
	float last_frame = 0.0f;

	FPS* fps = new FPS();

	// generate irradiance map
	{
		//matrix_t capture_views[] =
		//{
		//	matrix_lookat(vector_t(0.0f, 0.0f, 0.0f), vector_t(1.0f,  0.0f,  0.0f), vector_t(0.0f, -1.0f,  0.0f)),
		//	matrix_lookat(vector_t(0.0f, 0.0f, 0.0f), vector_t(-1.0f,  0.0f,  0.0f), vector_t(0.0f, -1.0f,  0.0f)),
		//	matrix_lookat(vector_t(0.0f, 0.0f, 0.0f), vector_t(0.0f,  1.0f,  0.0f), vector_t(0.0f,  0.0f,  1.0f)),
		//	matrix_lookat(vector_t(0.0f, 0.0f, 0.0f), vector_t(0.0f, -1.0f,  0.0f), vector_t(0.0f,  0.0f, -1.0f)),
		//	matrix_lookat(vector_t(0.0f, 0.0f, 0.0f), vector_t(0.0f,  0.0f,  1.0f), vector_t(0.0f, -1.0f,  0.0f)),
		//	matrix_lookat(vector_t(0.0f, 0.0f, 0.0f), vector_t(0.0f,  0.0f, -1.0f), vector_t(0.0f, -1.0f,  0.0f))
		//};

		//matrix_set_identity(&renderer_irradiance.transform.model);
		//matrix_set_perspective(&renderer_irradiance.transform.projection, radians(90.0f), 1.0f, 0.1f, 100.0f);
		//for (int i = 0; i < 6; i++) {
		//	renderer_irradiance.transform.view = capture_views[i];
		//	renderer_irradiance.transform_update();

		//	draw_light(renderer_irradiance);
		//}
	}

	while (window.screen_exit[0] == 0 && window.screen_keys[VK_ESCAPE] == 0) {
		fps->Print_FPS();

		//ʱ��,ʹ�ƶ��ٶȲ���֡�ʱ仯
		//������,��ʱ����
		float current_frame = time_get();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;
		camera.speed = 0.5f;

		//���
		mouse_callback(camera);
		gl_x_offset = 0; gl_y_offset = 0;

		window.screen_dispatch();
		renderer.clear();

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

		if (window.screen_keys[KEY_G]) {
			renderer.enable_gamma = renderer.enable_gamma ? false : true;
		}

		if (window.screen_keys[KEY_9]) {
			point_light_right.radiance = point_light_right.radiance + color_t(100);
		}
		if (window.screen_keys[KEY_0]) {
			point_light_right.radiance = point_light_right.radiance - color_t(100);
		}
		if (window.screen_keys[KEY_1]) {
			point_light_left.radiance = point_light_left.radiance + color_t(100);
		}
		if (window.screen_keys[KEY_2]) {
			point_light_left.radiance = point_light_left.radiance - color_t(100);
		}

		if (window.screen_keys[KEY_Z]) {
			renderer.only_albedo = renderer.only_albedo ? false : true;
		}

		//if (window.screen_keys[KEY_Y]) { g_fov -= 0.04f; }
		//if (window.screen_keys[KEY_H]) { g_fov += 0.04f; }
		//g_fov = CMID(g_fov, 1.0f, 60.0f);
		camera.fov = g_fov;

		//���������
		camera.target = camera.pos + camera.front;
		matrix_t view = camera.set_lookat(camera.pos, camera.target, camera.up);
		renderer.transform.view = view;

		matrix_set_perspective(&renderer.transform.projection, g_fov, aspect, 0.1f, 100.0f);

		matrix_t model;
		matrix_set_identity(&model);

		renderer_sky.transform = renderer.transform;
		renderer_sky.transform.model = model;
		renderer_sky.transform_update();

		for (int j = 0; j < 4; j++) {
			renderer_sky.transform.view.m[3][j] = 0;
			renderer_sky.transform.view.m[j][3] = 0;
		}
		renderer_sky.transform.view.m[3][3] = 1;

#ifdef DRAW_SKYBOX
		draw_skybox(renderer_sky);
#endif // DRAW_SKYBOX



		renderer.view_pos = camera.pos;

		matrix_set_identity(&model);
		//model = matrix_scale(model, { 0.02f,0.02f,0.02f,1 });
		//model = matrix_scale(model, { 0.5f,0.5f,0.5f,1 });
		model = model * matrix_rotate_build(radians(90), { 1.0f,0.0f,0.0f,1 });
		//model = model * matrix_rotate_build(radians(30), { 0.0f,1.0f,0.0f,1 });
		//model = matrix_translate(model, { 0.0f,-1.5f,0.0f,1 });
		renderer.transform.model = model;
		renderer.transform_update();

#ifdef DRAW_MODEL
		models.draw(renderer);
#endif // DRAW_MODEL


		//����Դ
		renderer_light.transform = renderer.transform;
		matrix_set_identity(&model);
		model = matrix_scale(model, { 0.2,0.2,0.2,1 });
		model = matrix_translate(model, point_light_right.pos);
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


