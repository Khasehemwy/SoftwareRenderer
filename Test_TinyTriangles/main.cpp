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

int main()
{
	Window window;
	window.screen_init(512, 512, _T("SoftwareRenderer - Tiny"));

	Renderer_Rasterization renderer;
	renderer.init(window.screen_width, window.screen_height, window.screen_fb);

	float aspect = (float)renderer.width / ((float)renderer.height);

	Camera camera;
	camera.fov = g_fov;
	float posz = -4;
	float posx = 0;
	camera.init_target_zero({ posx,0.5,posz,1 });
	camera.front = { 0,0,1,1 };

	float angle = 0;
	vector_t rotate_axis = { 1,-0.5,0.5,1 };
	renderer.camera = &camera;
	//renderer.render_state = RENDER_STATE_WIREFRAME;
	renderer.render_state = RENDER_STATE_TEXTURE;
	renderer.features[RENDER_FEATURE_LIGHT] = false;
	renderer.features[RENDER_FEATURE_AUTO_NORMAL] = false;
	renderer.features[RENDER_FEATURE_CVV_CLIP] = true;
	//renderer.render_state = RENDER_STATE_TEXTURE;
	renderer.features[RENDER_FEATURE_BACK_CULLING] = true;
	renderer.render_shader_state = RENDER_SHADER_PIXEL_EDGEEQUATION;

	Model models("../resources/Candle_Holder/Candle_Holder.fbx");

	//时间
	float delta_time = 0.0f;
	float last_frame = 0.0f;

	FPS* fps = new FPS();

	while (window.screen_exit[0] == 0 && window.screen_keys[VK_ESCAPE] == 0) {
		fps->Print_FPS();

		//std::thread t_operation([&]() {std::cout << camera.pos.x << "," << camera.pos.y << "," << camera.pos.z <<"\n"; });
		//t_operation.detach();

		//时间,使移动速度不受帧率变化
		//不好用,暂时舍弃
		float current_frame = time_get();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;
		camera.speed = 0.05f;

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

		//if (window.screen_keys[KEY_Y]) { g_fov -= 0.04f; }
		//if (window.screen_keys[KEY_H]) { g_fov += 0.04f; }
		//g_fov = CMID(g_fov, 1.0f, 60.0f);
		camera.fov = g_fov;

		//更新摄像机
		camera.target = camera.pos + camera.front;
		matrix_t view = camera.set_lookat(camera.pos, camera.target, camera.up);
		renderer.transform.view = view;

		matrix_set_perspective(&renderer.transform.projection, g_fov, aspect, 0.1f, 100.0f);

		matrix_t model;

		for (int i = -2; i <= 2; i++) {
			for (int j = -2; j <= 2; j++) {
				matrix_set_identity(&model);
				model = matrix_scale(model, { 0.002f,0.002f,0.002f,1 });
				model = matrix_translate(model, { i / 2.0f,j / 2.0f,0.0f,1 });
				renderer.transform.model = model;
				renderer.transform_update();

#ifdef DRAW_MODEL
				models.draw(renderer);
#endif // DRAW_MODEL

			}
		}

		renderer.primitive_id = 0;

		renderer.draw_line(10, 10, 20, 10, 0x0);
		renderer.draw_line(10, 10, 10, 20, 0x0);

		renderer.Rendering();

		window.screen_update();
	}

	return 0;
}


