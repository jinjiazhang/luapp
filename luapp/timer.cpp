#include "timer.h"
#include <assert.h>

timer::timer(int curtime)
{
    last_tid_ = 0;
    last_time_ = curtime;
}

timer::~timer()
{
}

int timer::update(int64_t curtime)
{
    last_time_ = curtime;
    return 0;
}

int timer::insert(int second, callback* handle)
{
    tnode* node = new tnode();
    node->tid = ++last_tid_;
    node->expire = last_time_ + second * 1000;
    node->handle = handle;
    node->next = NULL;

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

    node->expire = last_time_ + second * 1000;
    assert(insert(node));
    return true;
}

timer::tlist* timer::select(tnode* node)
{
    return NULL;
}

bool timer::insert(tnode* node)
{
    tlist* list = this->select(node);
    if (list == NULL) {
        return false;
    }

    node->next = list->head;
    list->head = node;
    return true;
}

bool timer::remove(tnode* node)
{
    tlist* list = this->select(node);
    if (list == NULL) {
        return false;
    }

    tnode* last = NULL;
    tnode* temp = list->head;
    while (temp != NULL)
    {
        if (temp->tid == node->tid)
        {
            if (last != NULL)
                last->next = node->next;
            else
                list->head = node->next;
        }
        last = temp;
        temp = temp->next;
    }
    return true;
}

void timer::timeout(tlist* list)
{
    tnode* temp = list->head;
    list->head = NULL;
    while (temp != NULL)
    {
        tnode* node = temp;
        temp = temp->next;
        node->handle->timeout(node->tid);
        nodes_.erase(node->tid);
        delete node;
    }
}