#include <string>
#include <chrono>
#include <thread>
#include <sstream>

#include "plat.h"


void sys_sleep(int time)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

int64_t sys_mstime()
{
    return std::chrono::system_clock::now().time_since_epoch().count() / 10000;
}

int64_t sys_filetime(const char* file)
{
    struct stat info;
    if (stat(file, &info) != 0) {
        return 0;
    }
    return info.st_mtime;
}

std::string lua_showstack(lua_State* L)
{
    std::ostringstream out;
    int n = lua_gettop(L);  /* number of arguments */
    lua_getglobal(L, "tostring");
    for (int i = 1; i <= n; i++) {
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        if (i > 1) out << "  ";
        out << lua_tostring(L, -1);  /* get result */
        lua_pop(L, 1);  /* pop result */
    }
    out << std::endl;
    return out.str();
}

int lua_loginfo(lua_State* L)
{
    luapp_info(lua_showstack(L).c_str());
    return 0;
}

int lua_logwarn(lua_State* L)
{
    luapp_warn(lua_showstack(L).c_str());
    return 0;
}

int lua_logerror(lua_State* L)
{
    luapp_error(lua_showstack(L).c_str());
    return 0;
}

int lua_logfatal(lua_State* L)
{
    luapp_fatal(lua_showstack(L).c_str());
    return 0;
}

void app_daemon()
{
#ifdef linux

#endif
}

EXPORT_CFUNC(sys_sleep)
EXPORT_CFUNC(sys_mstime)
EXPORT_CFUNC(sys_filetime)
void luaL_opensys(lua_State* L)
{
    static const struct luaL_Reg sysLibs[] = {
        { "sleep", lua_sys_sleep },
        { "mstime", lua_sys_mstime },
        { "filetime", lua_sys_filetime },
        { NULL, NULL }
    };

    lua_newtable(L);
    luaL_setfuncs(L, sysLibs, 0);
    lua_setglobal(L, "sys");
}

void luaL_openplat(lua_State* L)
{
    lua_register(L, "log_info", lua_loginfo);
    lua_register(L, "log_warn", lua_logwarn);
    lua_register(L, "log_error", lua_logerror);
    lua_register(L, "log_fatal", lua_logfatal);
}