#ifndef _JINJIAZHANG_GWCLIENT_H_
#define _JINJIAZHANG_GWCLIENT_H_

#include "gwstruct.h"
#include "protonet/inetwork.h"
#include "lualib/lobject.h"

class gateway;
class gwclient : public lobject, public imanager
{
public:
    gwclient(lua_State* L, svrid_t svrid);
    ~gwclient();

    int number();
    bool init(gateway* manager, int number);

    int close(lua_State* L);
    int start(lua_State* L);
    int stop(lua_State* L);
    int transmit(lua_State* L);
    int broadcast(lua_State* L);
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int number, int error);
    virtual void on_closed(int number, int error);
    virtual void on_package(int number, char* data, int len);

private:
    void on_reg_svrid(char* data, int len);
    void on_remote_call(char* data, int len);
    void on_session_start(char* data, int len);
    void on_session_stop(char* data, int len);
    void on_transmit_data(char* data, int len);

private:
    svrid_t svrid_;
    svrid_t gateway_;
    int number_;
    inetwork* network_;
    gateway* manager_;

};

#endif