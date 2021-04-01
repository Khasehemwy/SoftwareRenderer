#pragma once
#include"Includes.h"

class Texture {
public:
	static const int max_size = 256;
	UINT32 texture[max_size][max_size];
	int w, h;

	void init();
};