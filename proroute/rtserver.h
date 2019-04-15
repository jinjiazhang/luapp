#ifndef _JINJIAZHANG_RTSERVER_H_
#define _JINJIAZHANG_RTSERVER_H_

#include "rtstruct.h"
#include "protonet/inetwork.h"
#include "lualib/lobject.h"

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
    svrid_t svrid_;
    int number_;
    routermgr* manager_;
};

#endif