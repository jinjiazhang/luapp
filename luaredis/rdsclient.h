#ifndef _JINJIAZHANG_RDSCLIENT_H_
#define _JINJIAZHANG_RDSCLIENT_H_

#include "lualib/lobject.h"
#include "protonet/network.h"

class luaredis;
struct inetwork;
struct redisReply;
struct redisAsyncContext;
class rdsclient : public lobject, public iobject
{
public:
    struct taskdata
    {
        int token = 0;
        int method = 0;
    };
public:
    rdsclient(lua_State* L, luaredis* rds);
	~rdsclient();

	bool init(redisAsyncContext* context);
    void add_event(int events);
    void del_event(int events);

    virtual void on_event(int events);
    void on_connect(int status);
    void on_disconnect(int status);
    void on_reply(redisReply* reply, void* privdata);

    int rds_command(lua_State* L);
    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:
    int last_token_;
    luaredis* luaredis_;
    redisAsyncContext* context_;
};

#endif