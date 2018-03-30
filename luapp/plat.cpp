#include <string>
#include <chrono>
#include <thread>
#include <stdio.h>

#include "plat.h"

void app_sleep(int time)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

int64_t app_time()
{
	return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

void app_daemon()
{
#ifdef linux

#endif
}
