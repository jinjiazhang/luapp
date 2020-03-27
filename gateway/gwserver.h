#ifndef _JINJIAZHANG_GWSERVER_H_
#define _JINJIAZHANG_GWSERVER_H_

#include "gwstruct.h"
#include "protonet/inetwork.h"
#include "lualib/lobject.h"
#include <unordered_map>

class gwproxy;
class gateway;
class gwserver : public lobject, public imanager
{
public:
    gwserver(lua_State* L, svrid_t svrid);
    ~gwserver();

    int number();
    inetwork* network();
    bool init(gateway* manager, int number);

    int open(lua_State* L);
    int close(lua_State* L);
    int start(lua_State* L);
    int stop(lua_State* L);
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int number, int error);
    virtual void on_closed(int number, int error);
    virtual void on_package(int number, char* data, int len);

    void reg_connid(connid_t connid, gwproxy* proxy);
    void unreg_connid(connid_t connid);
    bool is_accepted(connid_t connid);
    void transmit_data(connid_t connid, char* data, int len);

private:
    svrid_t num_to_svrid(int number);
    int svrid_to_num(svrid_t svrid);
    svrid_t connid_to_svrid(connid_t connid);
    gwproxy* connid_to_proxy(connid_t connid);

    void on_reg_svrid(int number, char* data, int len);
    void on_remote_call(int number, char* data, int len);
    void on_start_session(int number, char* data, int len);
    void on_stop_session(int number, char* data, int len);
    void on_transmit_data(int number, char* data, int len);
    void on_broadcast_data(int number, char* data, int len);
    void on_multicast_data(int number, char* data, int len);

private:
    typedef std::unordered_map<svrid_t, int> svrid_num_map;
    typedef std::unordered_map<int, svrid_t> num_svrid_map;
    typedef std::unordered_map<connid_t, svrid_t> conn_svrid_map;
    typedef std::unordered_map<connid_t, gwproxy*> conn_proxy_map;

    svrid_t svrid_;
    int number_;
    inetwork* network_;
    gateway* manager_;

    svrid_num_map svrid_num_map_;
    num_svrid_map num_svrid_map_;
    conn_svrid_map conn_svrid_map_;
    conn_proxy_map conn_proxy_map_;
};

#endif