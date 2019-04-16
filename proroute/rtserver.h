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

    int number();
    bool init(routermgr* manager, int number);
    void close();

    int set_group(lua_State* L);
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int number, int error);
    virtual void on_closed(int number, int error);
    virtual void on_package(int number, char* data, int len);

private:
    svrid_t num_to_svrid(int number);
    int svrid_to_num(svrid_t svrid);
    int roleid_to_num(roleid_t roleid);

    void on_reg_svrid(int number, char* data, int len);
    void on_reg_roleid(int number, char* data, int len);
    void on_unreg_roleid(int number, char* data, int len);

    void on_call_server(int number, char* data, int len);
    void on_forward_svrid(int number, char* data, int len);
    void on_forward_roleid(int number, char* data, int len);
    void on_forward_group(int number, char* data, int len);
    void on_forward_random(int number, char* data, int len);

private:
    typedef std::unordered_map<svrid_t, int> svrid_num_map;
    typedef std::unordered_map<int, svrid_t> num_svrid_map;
    typedef std::unordered_map<roleid_t, int> roleid_num_map;

    typedef std::set<svrid_t> svrid_list;
    typedef std::unordered_map<svrid_t, group_t> svrid_group_map;
    typedef std::unordered_map<group_t, svrid_list> group_svrids_map;

    svrid_t svrid_;
    int number_;
    inetwork* network_;
    routermgr* manager_;

    svrid_num_map svrid_num_map_;
    num_svrid_map num_svrid_map_;
    roleid_num_map roleid_num_map_;
    svrid_group_map svrid_group_map_;
    group_svrids_map group_svrids_map_;
};

#endif