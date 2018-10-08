#ifndef _JINJIAZHANG_TIMER_H_
#define _JINJIAZHANG_TIMER_H_

#include <map>
#include <list>
#include "plat.h"

#define TIME_UNIT       (250)
#define LEVEL_DEPTH     (4)
#define LEVEL_BITS      (8)
#define LEVEL_SIZE      (1 << LEVEL_BITS)
#define LEVEL_MASK      (LEVEL_SIZE - 1)
#define WHEEL_SIZE      (LEVEL_DEPTH * LEVEL_SIZE)

class timer
{
public:
    struct callback
    {
        virtual void timeout(int tid) = 0;
    };

    struct tnode
    {
        int tid;
        int index;
        int second;
        bool repeat;
        int64_t expire;
        callback* handle;
    };

public:
    timer(int64_t current);
    ~timer();

public:
    int update(int64_t current);
    int insert(callback* handle, int second, bool repeat);
    bool remove(int tid);
    bool change(int tid, int second);

private:
    void forward();
    void timeout(int index);
    void movlist(int index);
    int  select(tnode* node);
    bool insert(tnode* node);
    bool remove(tnode* node);

private:
    typedef std::map<int, tnode*> node_map;
    typedef std::list<tnode*> node_list;
    node_map nodes_;
    node_list wheels_[WHEEL_SIZE];

    int last_tid_;
    int64_t current_;
    int64_t jiffies_;
};

#endif