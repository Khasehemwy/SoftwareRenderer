#pragma once
#include"Includes.h"
class Camera
{
public:
	vector_t pos;
	vector_t up, right;
	vector_t front;
	vector_t target;

	float speed = 0.05f;
	float fov = 45.0f;
	float z_near = 0.1f, z_far = 100.0f;

	void init_target_zero(const vector_t& pos);
	matrix_t set_lookat(const vector_t& eye, const vector_t& at, const vector_t& up);
};

