#include "timer.h"

timer::timer(int curtime)
{
}

timer::~timer()
{
}

int timer::update(int64_t curtime)
{
    return 0;
}

int timer::insert(int second, callback * obj)
{
    return 0;
}

bool timer::remove(int tid)
{
    return false;
}

bool timer::change(int tid, int second)
{
    return false;
}
