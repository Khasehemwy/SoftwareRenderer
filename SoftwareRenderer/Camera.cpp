#include "Camera.h"

matrix_t Camera::set_lookat(const vector_t& pos, const vector_t& target, const vector_t& up)
{
	vector_t Xaxis, Yaxis, Zaxis;

	Zaxis = vector_normalize(target - pos);
	Xaxis = vector_normalize(vector_cross(up,Zaxis));
	Yaxis = vector_cross(Zaxis,Xaxis);
	//Xaxis = vector_normalize(vector_cross(Zaxis,up));
	//Yaxis = vector_cross(Xaxis,Zaxis);

	matrix_t m;
	m.m[0][0] = Xaxis.x;
	m.m[1][0] = Xaxis.y;
	m.m[2][0] = Xaxis.z;
	m.m[3][0] = -(Xaxis*(pos));
	 
	m.m[0][1] = Yaxis.x;
	m.m[1][1] = Yaxis.y;
	m.m[2][1] = Yaxis.z;
	m.m[3][1] = -(Yaxis*(pos));
	 
	m.m[0][2] = Zaxis.x;
	m.m[1][2] = Zaxis.y;
	m.m[2][2] = Zaxis.z;
	m.m[3][2] = -(Zaxis*(pos));
	 
	m.m[0][3] = m.m[1][3] = m.m[2][3] = 0.0f;
	m.m[3][3] = 1.0f;
    return m;
}

void Camera::init_target_zero(const vector_t& pos)
{
	this->camera_pos = pos; this->camera_pos.w = 1;
	this->target = { 0, 0, 0, 1 };
	this->camera_up = { 0, 0, 1, 1 };
}
