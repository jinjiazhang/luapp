#ifndef _JINJIAZHANG_LNETWORK_H_
#define _JINJIAZHANG_LNETWORK_H_

#include "inetwork.h"
#include "platform.h"
#include "lualib/lobject.h"

class lmanager;
class lnetwork : public lobject
{
public:
    lnetwork(lua_State* L);
    ~lnetwork();

    int update(int timeout);
    inetwork* impl();

    void push_manager(lmanager* manager);
    void pop_manager(lmanager* manager);
    void del_manager(lmanager* manager);
    virtual const luaL_Reg* get_libs();
        
    int listen(lua_State* L);
    int connect(lua_State* L);
    int send(lua_State* L);
    int close(lua_State* L);

private:
    typedef std::map<int, lmanager*> manager_map;
    typedef std::set<lmanager*> delete_set;

    manager_map managers_;
    delete_set deletes_;
    inetwork* network_;
};

#endif
