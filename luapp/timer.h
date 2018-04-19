#ifndef _JINJIAZHANG_TIMER_H_
#define _JINJIAZHANG_TIMER_H_

#include <map>
#include "plat.h"

class timer
{
public:
    struct callback
    {
        virtual void timeout(int tid) = 0;
    };

private:
    struct tnode
    {
        int tid;
        int64_t expire;
        callback* handle;
        tnode* next;
    };

    struct tlist
    {
        tnode* head;
    };

public:
    timer(int curtime);
    ~timer();

public:
    int update(int64_t curtime);
    int insert(int second, callback* handle);
    bool remove(int tid);
    bool change(int tid, int second);

private:
    tlist* select(tnode* node);
    bool insert(tnode* node);
    bool remove(tnode* node);
    void timeout(tlist* list);

private:
    typedef std::map<int, tnode*> node_map;
    node_map nodes_;

    int last_tid_;
    int64_t last_time_;
};

#endif