#pragma once
#include"Includes.h"
class Camera
{
public:
	vector_t camera_pos;
	vector_t camera_up, camera_direction, camera_right;
	vector_t target;

	void init_target_zero(const vector_t& pos);
	matrix_t set_lookat(const vector_t& pos, const vector_t& target, const vector_t& up);
};

