#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include "plat.h"

#ifdef _MSC_VER
#include <windows.h>
#endif

void set_title(unsigned int svrid, const char* name)
{
#ifdef _MSC_VER
    std::stringstream stream;
    stream << name << " --id=" << svrid_itos(svrid);
    SetConsoleTitle(stream.str().c_str());
#endif
}

// svrid string to int
unsigned int svrid_stoi(const std::string& text)
{
    unsigned int bytes[4] = {0};
    sscanf(text.c_str(), "%d.%d.%d.%d", &bytes[0], &bytes[1], &bytes[2], &bytes[3]);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3] << 0);
}

// svrid int to string
std::string svrid_itos(unsigned int svrid)
{
    std::stringstream stream;
    stream << ((svrid >> 24) & 0xFF) << '.';
    stream << ((svrid >> 16) & 0xFF) << '.';
    stream << ((svrid >>  8) & 0xFF) << '.';
    stream << ((svrid >>  0) & 0xFF);
    return stream.str();
}

void app_daemon()
{
#if defined(__linux__) || defined(__APPLE__)
    pid_t pid = fork();
    if (pid != 0)
        exit(0);

    setsid();
    umask(0);

    int null = open("/dev/null", O_RDWR);
    if (null != -1)
    {
        dup2(null, STDIN_FILENO);
        dup2(null, STDOUT_FILENO);
        dup2(null, STDERR_FILENO);
        close(null);
    }
#endif
}

void sys_sleep(int time)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

int64_t sys_mstime()
{
    std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

int64_t sys_filetime(const char* file)
{
    struct stat info;
    if (stat(file, &info) != 0) {
        return 0;
    }
    return info.st_mtime;
}

std::string sys_md5file(const char* file)
{
    return std::to_string(sys_filetime(file));
}

EXPORT_CFUNC(sys_sleep)
EXPORT_CFUNC(sys_mstime)
EXPORT_CFUNC(sys_filetime)
EXPORT_CFUNC(sys_md5file)
int luaopen_system(lua_State* L)
{
    static const struct luaL_Reg sysLibs[] = {
        { "sleep", lua_sys_sleep },
        { "mstime", lua_sys_mstime },
        { "filetime", lua_sys_filetime },
        { "md5file", lua_sys_md5file },
        { NULL, NULL }
    };

    lua_newtable(L);
    luaL_setfuncs(L, sysLibs, 0);
    lua_setglobal(L, "sys");
    return 0;
}

std::string lua_stackview(lua_State* L)
{
    std::ostringstream out;
    int n = lua_gettop(L);  /* number of arguments */
    lua_getglobal(L, "tostring");
    for (int i = 1; i <= n; i++) {
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        if (i > 1) out << ", ";
        out << lua_tostring(L, -1);  /* get result */
        lua_pop(L, 1);  /* pop result */
    }
    return out.str();
}

int lua_logtrace(lua_State* L)
{
    put_trace(lua_stackview(L).c_str());
    return 0;
}

int lua_logdebug(lua_State* L)
{
    put_debug(lua_stackview(L).c_str());
    return 0;
}

int lua_loginfo(lua_State* L)
{
    put_info(lua_stackview(L).c_str());
    return 0;
}

int lua_logwarn(lua_State* L)
{
    put_warn(lua_stackview(L).c_str());
    return 0;
}

int lua_logerror(lua_State* L)
{
    put_error(lua_stackview(L).c_str());
    return 0;
}

int lua_logfatal(lua_State* L)
{
    put_fatal(lua_stackview(L).c_str());
    return 0;
}

int luaopen_protolog(lua_State* L)
{
    lua_register(L, "print", lua_loginfo);
    lua_register(L, "log_trace", lua_logtrace);
    lua_register(L, "log_debug", lua_logdebug);
    lua_register(L, "log_info", lua_loginfo);
    lua_register(L, "log_warn", lua_logwarn);
    lua_register(L, "log_error", lua_logerror);
    lua_register(L, "log_fatal", lua_logfatal);
    return 0;
}
