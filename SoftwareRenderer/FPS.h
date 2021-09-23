#pragma once
#include "Includes.h"

class FPS {
private:
	float fps = 0.0f;
	unsigned int frame_count = 0;
	std::chrono::system_clock::time_point last_time = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point cur_time = std::chrono::system_clock::now();
public:
	void Print_FPS();
};