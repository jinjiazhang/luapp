#include "gwconn.h"

gwconn::gwconn(int connid)
{
    connid_ = connid;
    encrypt_ = false;
    key_recv_ = false;
}

gwconn::~gwconn()
{

}

bool gwconn::init(url_info* args, gwctx* ctx)
{
    return true;
}

void gwconn::close()
{
    
}

void gwconn::on_accept(int connid, int error)
{

}

void gwconn::on_closed(int connid, int error)
{

}

static char output[1024*1024];
void gwconn::send(const void* data, int len)
{
    if (!key_recv_)
    {
        send_cache_.push_back(std::string((const char*)data, len));
        return;
    }

    if (encrypt_)
    {
        int outlen = sizeof(output);
        cipher_.encrypt((const char*)data, len, output, &outlen);
        raw_send(output, outlen);
    }
    else
    {
        raw_send(data, len);
    }
}

void gwconn::raw_send(const void* data, int len)
{

}

void gwconn::recv_key(const char* data, int len)
{
    encrypt_ = data[0];
    if (encrypt_)
    {
        cipher_.init(data + 1, len - 1);
    }
}

void gwconn::on_package(int connid, char* data, int len)
{
    if (!key_recv_)
    {
        recv_key(data, len);
        key_recv_ = true;

        for (std::string& cache : send_cache_)
        {
            send(cache.data(), cache.size());
        }
        on_accept(connid, 0);
        return;
    }

    if (encrypt_)
    {
        int outlen = sizeof(output);
        cipher_.decrypt((const char*)data, len, output, &outlen);
        raw_package(connid, output, outlen);
    }
    else
    {
        raw_package(connid, data, len);
    }
}

void gwconn::raw_package(int connid, char* data, int len)
{

}