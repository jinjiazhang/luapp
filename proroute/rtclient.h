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

    int netid();
    bool init(routermgr* manager, int netid);

    int reg_role(lua_State* L);
    int unreg_role(lua_State* L);
    int call_target(lua_State* L);
    int call_transmit(lua_State* L);
    int call_group(lua_State* L);
    int call_random(lua_State* L);
    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int netid, int error);
    virtual void on_closed(int netid, int error);
    virtual void on_package(int netid, char* data, int len);

private:
    void on_reg_svrid(char* data, int len);
    void on_remote_call(char* data, int len);
    void on_transmit_call(char* data, int len);

private:
    svrid_t svrid_;
    svrid_t router_;
    int netid_;
    inetwork* network_;
    routermgr* manager_;
};

#endif