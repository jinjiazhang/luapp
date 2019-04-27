#ifndef _JINJIAZHANG_SQLPOOL_H_
#define _JINJIAZHANG_SQLPOOL_H_

#include "lualib/lobject.h"
#include "safe_queue.h"

class mysqlmgr;
class sqlpool : public lobject
{
public:
    sqlpool(lua_State* L);
    ~sqlpool();

    bool init(mysqlmgr* sqlmgr);

    int connect(lua_State* L);
    int sql_select(lua_State* L);
    int sql_insert(lua_State* L);
    int sql_update(lua_State* L);
    int sql_delete(lua_State* L);
    int sql_execute(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:
    mysqlmgr* sqlmgr_;
    safe_queue<std::function<void()>> req_queue_;
    safe_queue<std::function<void()>> rsp_queue_;
};

#endif