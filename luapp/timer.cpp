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
    while (current_ < current)
    {
        forward();
        current_ += TIME_UNIT;
    }
    return 0;
}

int timer::insert(callback* handle, int second)
{
    tnode* node = new tnode();
    node->tid = ++last_tid_;
    node->index = -1;
    node->expire = jiffies_ + second * 1000 / TIME_UNIT;
    node->handle = handle;

    bool succeed = insert(node);
    assert(succeed);
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
    bool succeed = remove(node);
    assert(succeed);
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
    bool succeed = remove(node);
    assert(succeed);

    node->expire = jiffies_ + second * 1000 / TIME_UNIT;
    succeed = insert(node);
    assert(succeed);
    return true;
}

void timer::forward()
{
    timeout(jiffies_ & LEVEL_MASK);
    int64_t point = ++jiffies_;
    int level = 0;
    while ((point & LEVEL_MASK) == 0 && level < LEVEL_DEPTH - 1)
    {
        int index = (level + 1) * LEVEL_SIZE + ((point >> LEVEL_BITS) & LEVEL_MASK);
        movlist(index);
        point >>= LEVEL_BITS;
        level++;
    }
}

int timer::select(tnode* node)
{
    int64_t point = node->expire;
    int64_t delta = point - jiffies_;
    int64_t bound = 1 << LEVEL_BITS;
    for (int level = 0; level < LEVEL_DEPTH; level++)
    {
        if (delta < bound)
        {
            return level * LEVEL_SIZE + (point & LEVEL_MASK);
        }
        bound <<= LEVEL_BITS;
        point >>= LEVEL_BITS;
    }
    return -1;
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
    return true;
}

void timer::timeout(int index)
{
    node_list& list = wheels_[index];
    while (!list.empty())
    {
        tnode* node = list.front();
        list.pop_front();
        node->handle->timeout(node->tid);
        nodes_.erase(node->tid);
        delete node;
    }
}

void timer::movlist(int index)
{
    node_list list;
    wheels_[index].swap(list);
    while (!list.empty())
    {
        tnode* node = list.front();
        list.pop_front();
        insert(node);
    }
}