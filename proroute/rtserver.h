#ifndef _JINJIAZHANG_RTSERVER_H_
#define _JINJIAZHANG_RTSERVER_H_

#include "rtstruct.h"
#include "protonet/inetwork.h"
#include "lualib/lobject.h"
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
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int number, int error);
    virtual void on_closed(int number, int error);
    virtual void on_package(int number, char* data, int len);

private:
    int svrid_to_num(svrid_t svrid);
    svrid_t num_to_svrid(int number);
    void map_num_svrid(int number, svrid_t svrid);
    void clean_num_svrid(int number);

    int roleid_to_num(roleid_t roleid);
    void map_roleid_num(roleid_t roleid, int number);
    void clean_roleid_num(roleid_t roleid);

    void on_reg_svrid(int number, char* data, int len);
    void on_reg_roleid(int number, char* data, int len);
    void on_unreg_roleid(int number, char* data, int len);
    void on_forward_svrid(int number, char* data, int len);
    void on_forward_roleid(int number, char* data, int len);

private:
    typedef std::unordered_map<svrid_t, int> svrid_num_map;
    typedef std::unordered_map<int, svrid_t> num_svrid_map;
    typedef std::unordered_map<roleid_t, int> roleid_num_map;

    svrid_t svrid_;
    int number_;
    inetwork* network_;
    routermgr* manager_;

    svrid_num_map svrid_num_map_;
    num_svrid_map num_svrid_map_;
    roleid_num_map roleid_num_map_;
};

#endif