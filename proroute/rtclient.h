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

    int reg_roleid(lua_State* L);
    int unreg_roleid(lua_State* L);
    int call_target(lua_State* L);
    int call_client(lua_State* L);
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int number, int error);
    virtual void on_closed(int number, int error);
    virtual void on_package(int number, char* data, int len);

private:
    void on_reg_svrid(char* data, int len);
    void on_remote_call(char* data, int len);
    void on_forward_roleid(char* data, int len);

private:
    svrid_t svrid_;
    svrid_t router_;
    int number_;
    inetwork* network_;
    routermgr* manager_;
};

#endif