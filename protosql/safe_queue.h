#ifndef _STACKOVERFLOW_SAFE_QUEUE_H_
#define _STACKOVERFLOW_SAFE_QUEUE_H_
// COPY FROM: https://stackoverflow.com/a/16075550

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class safe_queue
{
public:
    safe_queue(void)
    {
    }

    ~safe_queue(void)
    {
    }

    void enqueue(T t)
    {
        std::lock_guard<std::mutex> lock(m);
        q.push(t);
        c.notify_one();
    }

    T dequeue(void)
    {
        std::unique_lock<std::mutex> lock(m);
        while (q.empty())
        {
            c.wait(lock);
        }
        T val = q.front();
        q.pop();
        return val;
    }

private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
};

#endif
