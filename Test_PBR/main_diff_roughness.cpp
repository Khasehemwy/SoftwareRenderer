#include"Includes.h"
#include"shader_skybox.h"
#include"shader_diff_roughness.h"

#define DRAW_SKYBOX
#define DRAW_MODEL

float cursor_yaw = 0.0f;
float cursor_pitch = 0.0f;
float cursor_last_x = 400, cursor_last_y = 300;
float gl_x_offset = 90.0f;
float gl_y_offset = 0.0f;
float g_fov = radians(45.0);
vector_t front;



void mouse_callback(Camera& camera)
{
	cursor_yaw += gl_x_offset;
	cursor_pitch += gl_y_offset;
	if (cursor_pitch > 89.0f) { cursor_pitch = 89.0f; }
	if (cursor_pitch < -89.0f) { cursor_pitch = -89.0f; }
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

void SetTexturesByName(Renderer& renderer, std::map<std::string, std::array < Texture*, 7 >>texs, std::string tex_name)
{
	renderer.Set_Texture("diffuse", texs[tex_name][0]);
	renderer.Set_Texture("ao", texs[tex_name][1]);
	renderer.Set_Texture("height", texs[tex_name][2]);
	renderer.Set_Texture("glossiness", texs[tex_name][3]);
	renderer.Set_Texture("metallic", texs[tex_name][4]);
	renderer.Set_Texture("normal", texs[tex_name][5]);
	renderer.Set_Texture("roughness", texs[tex_name][6]);
}

int main()
{
	Window window;
	window.screen_init(1200, 600, _T("SoftwareRenderer - PBR"));

	Renderer_PBR renderer;
	renderer.init(window.screen_width, window.screen_height, window.screen_fb);
	Renderer renderer_light;
	renderer_light.init(window.screen_width, window.screen_height, window.screen_fb);
	renderer_light.z_buffer = renderer.z_buffer;//因为深度缓存是每个Renderer独用的,但是现在想让它们一起显示.

	Renderer_Skybox renderer_sky;
	renderer_sky.init(window.screen_width, window.screen_height, window.screen_fb);

	float aspect = (float)renderer.width / ((float)renderer.height);

	Camera camera;
	camera.fov = g_fov;
	float posz = -4;
	float posx = 0;
	camera.init_target_zero({ posx,0,posz,1 });
	front = { 0,0,1,1 };
	camera.front = front;

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

	std::map<std::string, std::array < Texture*, 7 >>texs_sphere;
	std::string texture_name;
	const int box_num = 6;
	renderer.enable_gamma = true;

	std::string texture_names[box_num] = {
		"cgaxis_grey_and_orange_marble_23_84",
		"cgaxis_cow_fur_37_08",
		"cgaxis_stained_patterned_metal_26_98",
		"cgaxis_rock_wall_with_moss_40_45",
		"cgaxis_scratched_golden_metal_26_05",
		"cgaxis_raw_cracked_white_concrete_46_42",
	};

	for (int i = 0; i < box_num; i++) {
		texture_name = texture_names[i];
		texs_sphere[texture_name][0] = new Texture("../resources/" + texture_name + "/diffuse.jpg");
		texs_sphere[texture_name][1] = new Texture("../resources/" + texture_name + "/ao.jpg");
		texs_sphere[texture_name][2] = new Texture("../resources/" + texture_name + "/height.jpg");
		texs_sphere[texture_name][3] = new Texture("../resources/" + texture_name + "/glossiness.jpg");
		texs_sphere[texture_name][4] = new Texture("../resources/" + texture_name + "/metallic.jpg");
		texs_sphere[texture_name][5] = new Texture("../resources/" + texture_name + "/normal.jpg");
		texs_sphere[texture_name][6] = new Texture("../resources/" + texture_name + "/roughness.jpg");
	}


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
	for (int i = 0; i < 6; i++) {
		if (!std::filesystem::exists(texture_skybox_path[i])) {
			texture_skybox_path[i] = texture_skybox_path[i].string().substr(0, texture_skybox_path[i].string().size() - 3) + "hdr";
			renderer_sky.isHDR = true;
		}
	}

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
	renderer.Set_Texture("BRDF_LUT", &tex_brdf_lut);


	//光源
	Light point_light_right;
	point_light_right.pos = { 4,1,-3,1 };
	point_light_right.radiance = { 500.0f,500.0f,500.0f,1 };
	point_light_right.light_state = LIGHT_STATE_POINT;
	renderer.add_light(point_light_right);

	Light point_light_left;
	point_light_left.pos = { -5,0,-0.5,1 };
	point_light_left.radiance = { 100.0f,100.0f,100.0f,1 };
	point_light_left.light_state = LIGHT_STATE_POINT;
	renderer.add_light(point_light_left);

	//时间
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

		//时间,使移动速度不受帧率变化
		//不好用,暂时舍弃
		float current_frame = time_get();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;
		camera.speed = 0.5f;

		//鼠标
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

		//更新摄像机
		camera.target = camera.pos + camera.front;
		matrix_t view = camera.set_lookat(camera.pos, camera.target, camera.up);
		renderer.transform.view = view;
		renderer.view_pos = camera.pos;

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


		vector_t translates[6] = {
			{ -1.3,0.6,0,1 },
			{ 0,0.6,0,1 },
			{ 1.3,0.6,0,1 },
			{ -1.3,-0.6,0,1 },
			{ 0,-0.6,0,1 },
			{ 1.3,-0.6,0,1 }
		};

		for (int i = 0; i < box_num; i++) {
			//画球体

			matrix_set_identity(&model);
			//model = matrix_scale(model, { 0.02f,0.02f,0.02f,1 });
			model = matrix_scale(model, { 0.5f,0.5f,0.5f,1 });
			model = model * matrix_rotate_build(radians(90), { 1.0f,0.0f,0.0f,1 });
			model = matrix_translate(model, translates[i]);
			//model = model * matrix_rotate_build(radians(30), { 0.0f,1.0f,0.0f,1 });
			//model = matrix_translate(model, { 0.0f,-1.5f,0.0f,1 });
			renderer.transform.model = model;
			renderer.transform_update();

#ifdef DRAW_MODEL
			SetTexturesByName(renderer, texs_sphere, texture_names[4]);
			renderer.roughness_custom = (float)i / box_num;
			models.draw(renderer);
#endif // DRAW_MODEL
		}

		//画光源
		//renderer_light.transform = renderer.transform;
		//matrix_set_identity(&model);
		//model = matrix_scale(model, { 0.2,0.2,0.2,1 });
		//model = matrix_translate(model, point_light_right.pos);
		//renderer_light.transform.model = model;
		//renderer_light.transform_update();
		//draw_light(renderer_light);

		renderer.draw_line(10, 10, 20, 10, 0x0);
		renderer.draw_line(10, 10, 10, 20, 0x0);

		renderer.Rendering();

		window.screen_update();
	}

	return 0;
}


