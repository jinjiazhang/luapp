#ifndef _JINJIAZHANG_GWSERVER_H_
#define _JINJIAZHANG_GWSERVER_H_

#include "gwstruct.h"
#include "protonet/inetwork.h"
#include "lualib/lobject.h"
#include <unordered_map>

class gateway;
class gwserver : public lobject, public imanager
{
public:
    gwserver(lua_State* L, svrid_t svrid);
    ~gwserver();

    int number();
    bool init(gateway* manager, int number);

    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int number, int error);
    virtual void on_closed(int number, int error);
    virtual void on_package(int number, char* data, int len);

private:
    svrid_t num_to_svrid(int number);
    int svrid_to_num(svrid_t svrid);

    void on_reg_svrid(int number, char* data, int len);
    void on_remote_call(int number, char* data, int len);
    void on_start_session(int number, char* data, int len);
    void on_stop_session(int number, char* data, int len);
    void on_transmit_data(int number, char* data, int len);
    void on_broadcast_data(int number, char* data, int len);

private:
    typedef std::unordered_map<svrid_t, int> svrid_num_map;
    typedef std::unordered_map<int, svrid_t> num_svrid_map;

    svrid_t svrid_;
    int number_;
    inetwork* network_;
    gateway* manager_;

    svrid_num_map svrid_num_map_;
    num_svrid_map num_svrid_map_;
};

#endif