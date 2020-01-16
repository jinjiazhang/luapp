#include "buffer.h"

buffer::buffer(int init_size)
{
    init_size_ = init_size;
    if (init_size_ > 0)
    {
        buffer_ = (char*)malloc(init_size_);
        begin_ = buffer_;
        end_ = begin_;
        capacity_ = init_size_;
    }
    else
    {
        buffer_ = NULL;
        begin_ = NULL;
        end_ = NULL;
        capacity_ = 0;
    }
}

buffer::~buffer()
{
    if (buffer_) 
    {
        free(buffer_);
        buffer_ = NULL;
        begin_ = NULL;
        end_ = NULL;
        capacity_ = 0;
    }
}

char* buffer::data()
{
    return begin_;
}

int buffer::size()
{
    return end_ - begin_;
}

char* buffer::tail()
{
    return end_;
}

int buffer::space()
{
    return buffer_ + capacity_ - end_;
}

void buffer::reserve(int count)
{
    if (buffer_ + capacity_ - begin_ > count)
    {
        return;
    }

    int used = end_ - begin_;
    int bias = begin_ - buffer_; 

    count = std::max<int>(count, init_size_);
    int capacity = std::max<int>(min_buffer_size, capacity_);
    while (capacity < count) capacity *= 2;
        
    if (capacity_ < capacity)
    {
        buffer_ = (char*)realloc(buffer_, capacity);
        capacity_ = capacity;
    }

    if (used > 0 && bias > 0)
    {
        memmove(buffer_, buffer_ + bias, used);
    }

    begin_ = buffer_;
    end_ = begin_ + used;
}

void buffer::shrink()
{
    int used = end_ - begin_;
    if (used == 0)
    {
        if (buffer_)
        {
            free(buffer_);
            buffer_ = NULL;
            begin_ = NULL;
            end_ = NULL;
            capacity_ = 0;
        }
        return;
    }

    assert(false); // todo
}

bool buffer::push_data(char* data, int len)
{
    if (size() + len > max_buffer_size)
    {
        return false;
    }

    reserve(size() + len);
    memcpy(end_, data, len);
    end_ += len;
    return true;
}

bool buffer::push_data(iovec *iov, int cnt, int ignore)
{
    int pushed = 0;
    for (int i = 0; i < cnt; i++, iov++)
    {
        if (ignore < (int)iov->iov_len)
        {
            if (!push_data((char*)iov->iov_base + ignore, iov->iov_len - ignore))
            {
                end_ -= pushed;
                return false;
            }
            pushed += iov->iov_len - ignore;
        }
        ignore = std::max<int>(0, ignore - iov->iov_len);
    }
    return true;
}

bool buffer::pop_data(int len)
{
    if (len > size())
    {
        return false;
    }

    begin_ += len;
    return true;
}

bool buffer::pop_space(int len)
{
    if (len > space())
    {
        return false;
    }

    end_ += len;
    return true;
}

void buffer::trim_data()
{
    int used = end_ - begin_;
    int bias = begin_ - buffer_;
    if (used > 0 && bias > 0)
    {
        memmove(buffer_, buffer_ + bias, used);
    }

    begin_ = buffer_;
    end_ = begin_ + used;
}
