#ifndef _JINJIAZHANG_LMANAGER_H_
#define _JINJIAZHANG_LMANAGER_H_

#include "inetwork.h"
#include "lualib/lobject.h"

class lnetwork;
class lmanager : public lobject, public imanager
{
public:
    lmanager(lua_State* L);
    ~lmanager();

    int number();
    bool init(lnetwork* network, int number);
    void close();
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int number, int error);
    virtual void on_closed(int number, int error);
    virtual void on_package(int number, char* data, int len);

private:
    int number_;
    lnetwork* network_;
};

#endif