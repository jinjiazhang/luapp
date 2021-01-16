#ifndef _JINJIAZHANG_RTSERVER_H_
#define _JINJIAZHANG_RTSERVER_H_

#include "rtstruct.h"
#include "protonet/inetwork.h"
#include "lualib/lobject.h"
#include <set>
#include <unordered_map>

class routermgr;
class rtserver : public lobject, public imanager
{
public:
    rtserver(lua_State* L, svrid_t svrid);
    ~rtserver();

    int netid();
    bool init(routermgr* manager, int netid);

    int set_group(lua_State* L);
    int call_target(lua_State* L);
    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int netid, int error);
    virtual void on_closed(int netid, int error);
    virtual void on_package(int netid, char* data, int len);

private:
    svrid_t netid_to_svrid(int netid);
    int svrid_to_netid(svrid_t svrid);
    int roleid_to_netid(roleid_t roleid, group_t group);

    void on_reg_svrid(int netid, char* data, int len);
    void on_reg_roleid(int netid, char* data, int len);
    void on_unreg_roleid(int netid, char* data, int len);

    void on_call_server(int netid, char* data, int len);
    void on_forward_svrid(int netid, char* data, int len);
    void on_forward_roleid(int netid, char* data, int len);
    void on_forward_group(int netid, char* data, int len);
    void on_forward_random(int netid, char* data, int len);

private:
    typedef std::unordered_map<svrid_t, int> svrid_netid_map;
    typedef std::unordered_map<int, svrid_t> netid_svrid_map;

    typedef std::unordered_map<roleid_t, int> roleid_netid_map;
    typedef std::unordered_map<group_t, roleid_netid_map> transmit_netid_map;

    typedef std::set<svrid_t> svrid_list;
    typedef std::unordered_map<svrid_t, group_t> svrid_group_map;
    typedef std::unordered_map<group_t, svrid_list> group_svrids_map;

    svrid_t svrid_;
    int netid_;
    inetwork* network_;
    routermgr* manager_;

    svrid_netid_map svrid_netid_map_;
    netid_svrid_map netid_svrid_map_;
    transmit_netid_map transmit_netid_map_;
    svrid_group_map svrid_group_map_;
    group_svrids_map group_svrids_map_;
};

#endif