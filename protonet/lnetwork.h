#ifndef _JINJIAZHANG_LNETWORK_H_
#define _JINJIAZHANG_LNETWORK_H_

#include "inetwork.h"
#include "platform.h"
#include "lualib/lobject.h"

class lnetwork : public lobject
{
public:
    lnetwork(lua_State* L);
    ~lnetwork();

    int update(int timeout);
    inetwork* impl();

    void add_manager(int number, imanager* manager);
    void del_manager(int number);
    virtual const luaL_Reg* get_libs();
        
    int listen(lua_State* L);
    int connect(lua_State* L);
    int send(lua_State* L);
    int close(lua_State* L);

private:
    typedef std::map<int, imanager*> manager_map;
    manager_map managers_;
    inetwork* network_;
};

#endif
