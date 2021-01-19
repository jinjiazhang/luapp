#include "gwcmgr.h"

gwcmgr::gwcmgr()
{

}

gwcmgr::~gwcmgr()
{

}

gwcmgr * gwcmgr::instance()
{
    return nullptr;
}

int gwcmgr::open(const char * url, gwctx * ctx)
{
    return 0;
}

int gwcmgr::close(int connid)
{
    return 0;
}

int gwcmgr::send(int connid, const void * data, int len)
{
    return 0;
}

int gwcmgr::sendv(int netid, iobuf bufs[], int count)
{
    return 0;
}

int gwcmgr::update(int timeout)
{
    return 0;
}
