#include "rdsclient.h"
#include "luaredis.h"
#include "protonet/network.h"
#include "async.h"

rdsclient::rdsclient(lua_State* L, luaredis* rds) : lobject(L)
{
    luaredis_ = rds;
    context_ = nullptr;
}

rdsclient::~rdsclient()
{
    luaredis_ = nullptr;
    context_ = nullptr;
}

static void redis_add_read(void *privdata)
{
    rdsclient* client = (rdsclient*)privdata;
    client->add_event(EVENT_READ);
}

static void redis_del_read(void *privdata)
{
    rdsclient* client = (rdsclient*)privdata;
    client->del_event(EVENT_READ);
}

static void redis_add_write(void *privdata)
{
    rdsclient* client = (rdsclient*)privdata;
    client->add_event(EVENT_WRITE);
}

static void redis_del_write(void *privdata)
{
    rdsclient* client = (rdsclient*)privdata;
    client->del_event(EVENT_WRITE);
}

static void redis_cleanup(void *privdata)
{
    rdsclient* client = (rdsclient*)privdata;
    client->del_event(EVENT_READ | EVENT_WRITE);
}

static void redis_on_connect(const redisAsyncContext *ac, int status)
{
    rdsclient* client = (rdsclient*)ac->data;
    // client->on_connect(status);
}

static void redis_on_disconnect(const redisAsyncContext *ac, int status)
{
    rdsclient* client = (rdsclient*)ac->data;
    // client->on_disconnect(status);
}

static void redis_on_reply(redisAsyncContext *c, void *data, void *privdata)
{
    rdsclient* client = (rdsclient*)privdata;
    // client->on_reply(data);
}

bool rdsclient::init(redisAsyncContext* context)
{
    context_ = context;
    context->ev.data = this;
    context->ev.addRead = redis_add_read;
    context->ev.delRead = redis_del_read;
    context->ev.addWrite = redis_add_write;
    context->ev.delWrite = redis_del_write;
    context->ev.cleanup = redis_cleanup;

    redisAsyncSetConnectCallback(context, redis_on_connect);
    redisAsyncSetDisconnectCallback(context, redis_on_disconnect);
    return true;
}

void rdsclient::add_event(int events)
{
    network* impl = dynamic_cast<network*>(luaredis_->get_network());
    if (impl != nullptr)
    {
        impl->add_event(this, context_->c.fd, events);
    }
}

void rdsclient::del_event(int events)
{
    network* impl = dynamic_cast<network*>(luaredis_->get_network());
    if (impl != nullptr)
    {
        impl->del_event(this, context_->c.fd, events);
    }
}

void rdsclient::on_event(int events)
{
    if (events | EVENT_READ)
    {
        redisAsyncHandleRead(context_);
    }
    if (events | EVENT_WRITE)
    {
        redisAsyncHandleWrite(context_);
    }
}

int rdsclient::command(lua_State* L)
{
    const char* cmd = lua_tostring(L, 1);
    int ret = redisAsyncCommand(context_, redis_on_reply, this, cmd);
    lua_pushinteger(L, ret);
    return 1;
}

int rdsclient::close(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(rdsclient, command)
EXPORT_OFUNC(rdsclient, close)
const luaL_Reg* rdsclient::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(rdsclient, command) },
        { IMPORT_OFUNC(rdsclient, close) },
        { NULL, NULL }
    };
    return libs;
}