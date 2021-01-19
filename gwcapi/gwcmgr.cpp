#include "gwcmgr.h"
#include "gwconn.h"

gwcmgr::gwcmgr()
{
    last_connid_ = 0;
}

gwcmgr::~gwcmgr()
{

}

gwcmgr* gwcmgr::instance()
{
    static gwcmgr mgr;
    return &mgr;
}

gwconn* gwcmgr::get_conn(int connid)
{
    conn_map::iterator it = conn_map_.find(connid);
    if (it == conn_map_.end())
    {
        return nullptr;
    }
    return it->second;
}

int gwcmgr::open(const char* url, gwctx* ctx)
{
    url_info args;
    memset(&args, 0, sizeof(args));

    if (!parse_url(url, &args))
    {
        return -1;
    }

    int connid = ++last_connid_;
    gwconn* conn = new gwconn(connid);
    if (!conn->init(&args, ctx))
    {
        return -1;
    }

    conn_map_.insert(std::make_pair(connid, conn));
    return connid;
}

int gwcmgr::close(int connid)
{
    gwconn* conn = get_conn(connid);
    if (conn == nullptr)
    {
        return -1;
    }

    conn->close();
    delete conn;
    conn_map_.erase(connid);
    return 0;
}

int gwcmgr::send(int connid, const void* data, int len)
{
    gwconn* conn = get_conn(connid);
    if (conn == nullptr)
    {
        return -1;
    }
    
    return 0;
}

int gwcmgr::sendv(int connid, iobuf bufs[], int count)
{
    gwconn* conn = get_conn(connid);
    if (conn == nullptr)
    {
        return -1;
    }

    for (int i = 0; i < count; i++)
    {
        conn->send(bufs[i].data, bufs[i].len);
    }
    return 0;
}

int gwcmgr::update(int timeout)
{
    return 0;
}
