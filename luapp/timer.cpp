#include "timer.h"
#include <assert.h>

timer::timer(int64_t current)
{
    last_tid_ = 0;
    current_ = current;
    jiffies_ = 0;
}

timer::~timer()
{
}

int timer::update(int64_t current)
{
    while (current_ + LEVEL_UNIT < current)
    {
        forward();
        current_ += LEVEL_UNIT;
    }
    return 0;
}

int timer::insert(int second, callback* handle)
{
    tnode* node = new tnode();
    node->tid = ++last_tid_;
    node->index = -1;
    node->expire = jiffies_ + second * 1000 / LEVEL_UNIT;
    node->handle = handle;

    assert(insert(node));
    nodes_.insert(std::make_pair(node->tid, node));
    return node->tid;
}

bool timer::remove(int tid)
{
    node_map::iterator it = nodes_.find(tid);
    if (it == nodes_.end())
    {
        return false;
    }

    tnode* node = it->second;
    assert(remove(node));
    nodes_.erase(it);
    delete node;
    return false;
}

bool timer::change(int tid, int second)
{
    node_map::iterator it = nodes_.find(tid);
    if (it == nodes_.end())
    {
        return false;
    }

    tnode* node = it->second;
    assert(remove(node));

    node->expire = jiffies_ + second * 1000 / LEVEL_UNIT;
    assert(insert(node));
    return true;
}

void timer::forward()
{

}

int timer::select(tnode* node)
{
    uint64_t delta = node->expire - jiffies_;
    for (int level = 0; level < LEVEL_DEPTH; level++)
    {
    }
    return 0;
}

bool timer::insert(tnode* node)
{
    int index = this->select(node);
    if (index < 0 || index >= WHEEL_SIZE)
    {
        return false;
    }

    node_list& list = wheels_[index];
    node->index = index;
    list.push_back(node);
    return true;
}

bool timer::remove(tnode* node)
{
    int index = node->index;
    if (index < 0 || index >= WHEEL_SIZE)
    {
        return false;
    }

    node_list& list = wheels_[index];
    list.remove(node);
    node->index = -1;
    return false;
}