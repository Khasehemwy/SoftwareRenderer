#include"Includes.h"

int Renderer::Rendering_RayTracing()
{
	color_t color_tmp = { 0,0,0,1 }, color = { 0,0,0,1 };
	matrix_t camera_to_world = matrix_get_inverse(this->transform.view);
	float scale = tan(radians(camera->fov * 0.5f));
	float aspect = width / height;

	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			color = { 0,0,0,1 };
			for (unsigned sy = 0; sy < 2; sy++) { // 2x2×ÓÏñËØ
				for (unsigned int sx = 0; sx < 2; sx++) {
					color_tmp = { 0,0,0,1 };
					for (int s = 0; s < raytracing_samples_num; s++) {
						float dx = rand_lr(0.0f, 1.0f) - 0.5f;//Ëæ»úÆ«ÒÆ
						float dy = rand_lr(0.0f, 1.0f) - 0.5f;

						float px = ((2 * (x + 0.5 + (sx - 0.5 + dx)) / width - 1)) * aspect * scale;
						float py = ((1 - 2 * (y + 0.5 + (sy - 0.5 + dy)) / height)) * scale;
						point_t pixel_pos = vector_t(px, py, 0.76, 1) * camera_to_world;

						vector_t view_dir = vector_normalize(pixel_pos - camera->pos);

						float tmp_f = 0.0f;
						color_tmp = color_tmp +
							Ray_Tracing({ camera->pos, view_dir }, 0, tmp_f) * (1.0 / raytracing_samples_num);
					}
					color = color + color_tmp * 0.25;
				}
			}

			this->draw_pixel(x, y, color_trans_255(color));
		}
	}

	return 0;
}
