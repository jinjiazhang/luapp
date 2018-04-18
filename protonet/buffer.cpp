#include "buffer.h"

buffer::buffer()
{
    preseted_ = 8096;
    capacity_ = 0;
    buffer_ = NULL;
    begin_ = NULL;
    end_ = NULL;
}

buffer::~buffer()
{
    if (buffer_) 
    {
        delete[] buffer_;
    }
}

void buffer::prepare()
{
    if (buffer_ == NULL)
    {
        buffer_ = new char[preseted_];
        begin_ = buffer_;
        end_ = begin_;
        capacity_ = preseted_;
    }
    else if (capacity_ != preseted_)
    {
        char* preset = new char[preseted_];
        int used = end_ - begin_;
        if (used > 0)
        {
            assert(preseted_ >= used);
            memcpy(preset, begin_, used);
        }
        delete[] buffer_;
        buffer_ = preset;
        begin_ = buffer_;
        end_ = begin_ + used;
        capacity_ = preseted_;
    }
}

char* buffer::data()
{
    prepare();
    return begin_;
}

int buffer::size()
{
    return end_ - begin_;
}

char* buffer::tail()
{
    prepare();
    return end_;
}

int buffer::space()
{
    prepare();
    return capacity_ - (end_ - buffer_);
}

bool buffer::expand(int size)
{
    if (size <= 0)
    {
        if (buffer_)
        {
            delete[] buffer_;
            buffer_ = NULL;
            begin_ = NULL;
            end_ = NULL;
            capacity_ = 0;
        }
        return true;
    }
    else if (size < 0x800000)
    {
        while (preseted_ < size)
        {
            preseted_ = preseted_ * 2;
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool buffer::push_data(char* data, int len)
{
    if (!expand(size() + len))
    {
        return false;
    }

    prepare();
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
    if (used > 0)
    {
        memmove(buffer_, begin_, used);
    }

    begin_ = buffer_;
    end_ = begin_ + used;
}
