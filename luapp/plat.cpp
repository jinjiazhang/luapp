#include <string>
#include <chrono>
#include <thread>
#include <stdio.h>

#include "plat.h"

void sys_sleep(int time)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

int64_t sys_mstime()
{
    return std::chrono::system_clock::now().time_since_epoch().count() / 10000;
}

void app_daemon()
{
#ifdef linux

#endif
}
