#ifndef _JINJIAZHANG_RDSCLIENT_H_
#define _JINJIAZHANG_RDSCLIENT_H_

#include "lualib/lobject.h"
#include "protonet/network.h"

class luaredis;
struct inetwork;
struct redisAsyncContext;
class rdsclient : public lobject, public iobject
{
public:
    rdsclient(lua_State* L, luaredis* rds);
	~rdsclient();

	bool init(redisAsyncContext* context);
    void add_event(int events);
    void del_event(int events);

    virtual void on_event(int events);

    int command(lua_State* L);
    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:
    luaredis* luaredis_;
    redisAsyncContext* context_;
};

#endif