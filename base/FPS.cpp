#include "FPS.h"

void FPS::Print_FPS()
{
	using namespace std::chrono;
	cur_time = system_clock::now();
	auto duration = duration_cast<microseconds>(cur_time - last_time);
	float duration_s = float(duration.count()) * std::chrono::microseconds::period::num / microseconds::period::den;

	if (duration_s > 1)//1秒之后开始统计FPS
	{
		fps = frame_count / duration_s;
		frame_count = 0;
		last_time = cur_time;
		std::cout << (int)fps << "\n";
	}

	++frame_count;
}
