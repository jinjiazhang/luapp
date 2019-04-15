#ifndef _JINJIAZHANG_RTCLIENT_H_
#define _JINJIAZHANG_RTCLIENT_H_

#include "rtstruct.h"
#include "protonet/inetwork.h"
#include "lualib/lobject.h"

class routermgr;
class rtclient : public lobject, public imanager
{
public:
    rtclient(lua_State* L, svrid_t svrid);
    ~rtclient();

    int number();
    bool init(routermgr* manager, int number);
    void close();
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int number, int error);
    virtual void on_closed(int number, int error);
    virtual void on_package(int number, char* data, int len);

private:
    svrid_t svrid_;
    int number_;
    inetwork* network_;
    routermgr* manager_;
};

#endif