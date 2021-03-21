#include "Camera.h"

matrix_t Camera::set_lookat(const vector_t* pos, const vector_t* target, const vector_t* up)
{
	vector_t Xaxis, Yaxis, Zaxis;

	Zaxis = (*target) - (*pos);
	Zaxis = vector_normalize(&Zaxis);
	Xaxis = vector_cross(up, &Zaxis);
	Xaxis = vector_normalize(&Xaxis);
	Yaxis = vector_cross(&Zaxis, &Xaxis);

	m->m[0][0] = xaxis.x;
	m->m[1][0] = xaxis.y;
	m->m[2][0] = xaxis.z;
	m->m[3][0] = -vector_dotproduct(&xaxis, eye);

	m->m[0][1] = yaxis.x;
	m->m[1][1] = yaxis.y;
	m->m[2][1] = yaxis.z;
	m->m[3][1] = -vector_dotproduct(&yaxis, eye);

	m->m[0][2] = zaxis.x;
	m->m[1][2] = zaxis.y;
	m->m[2][2] = zaxis.z;
	m->m[3][2] = -vector_dotproduct(&zaxis, eye);

	m->m[0][3] = m->m[1][3] = m->m[2][3] = 0.0f;
	m->m[3][3] = 1.0f;
    return matrix_t();
}
