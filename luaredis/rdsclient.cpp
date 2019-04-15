#include "rdsclient.h"
#include "luaredis.h"
#include "async.h"
#include "protonet/network.h"
#include "protolog/protolog.h"

rdsclient::rdsclient(lua_State* L, luaredis* rds) : lobject(L)
{
    last_token_ = 0;
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

static void redis_schedule_timer(void *privdata, struct timeval tv)
{
    rdsclient* client = (rdsclient*)privdata;
    put_warn("redis_schedule_timer todo add timer");
}

static void redis_on_connect(const redisAsyncContext *ac, int status)
{
    rdsclient* client = (rdsclient*)ac->ev.data;
    client->on_connect(status);
}

static void redis_on_disconnect(const redisAsyncContext *ac, int status)
{
    rdsclient* client = (rdsclient*)ac->ev.data;
    client->on_disconnect(status);
}

static void redis_on_reply(redisAsyncContext *ac, void *reply, void *privdata)
{
    rdsclient* client = (rdsclient*)ac->ev.data;
    client->on_reply((redisReply*)reply, privdata);
}

static void luaL_pushstatus(lua_State* L, redisReply* reply)
{
    if (reply == nullptr)
    {
        lua_pushinteger(L, -1);
    }
    else if (reply->type == REDIS_REPLY_ERROR)
    {
        lua_pushinteger(L, -2);
    }
    else
    {
        lua_pushinteger(L, 0);
    }
}

static void luaL_pushreply(lua_State* L, redisReply* reply)
{
    int reply_type = reply ? reply->type : 0;
    switch (reply_type)
    {
    case REDIS_REPLY_STRING:
    case REDIS_REPLY_STATUS:
    case REDIS_REPLY_ERROR:
        lua_pushlstring(L, reply->str, reply->len);
        break;
    case REDIS_REPLY_NIL:
        lua_pushnil(L);
        break;
    case REDIS_REPLY_INTEGER:
        lua_pushinteger(L, reply->integer);
        break;
    case REDIS_REPLY_ARRAY:
        lua_newtable(L);
        for (int i = 0; i < reply->elements; i++)
        {
            lua_pushinteger(L, i + 1);
            luaL_pushreply(L, reply->element[i]);
            lua_settable(L, -3);
        }
        break;
    default:
        lua_pushnil(L);
        break;
    }
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
    context->ev.scheduleTimer = redis_schedule_timer;

    redisAsyncSetConnectCallback(context, redis_on_connect);
    redisAsyncSetDisconnectCallback(context, redis_on_disconnect);
    return true;
}

void rdsclient::add_event(int events)
{
    network* impl = dynamic_cast<network*>(luaredis_->network());
    if (impl != nullptr)
    {
        impl->add_event(this, context_->c.fd, events);
    }
}

void rdsclient::del_event(int events)
{
    network* impl = dynamic_cast<network*>(luaredis_->network());
    if (impl != nullptr)
    {
        impl->del_event(this, context_->c.fd, events);
    }
}

void rdsclient::on_event(int events)
{
    if (events & EVENT_READ)
    {
        redisAsyncHandleRead(context_);
    }
    if (events & EVENT_WRITE)
    {
        redisAsyncHandleWrite(context_);
    }
}

void rdsclient::on_connect(int status)
{
    luaL_callfunc(L, this, "on_connect", status, context_->errstr);
}

void rdsclient::on_disconnect(int status)
{
    luaL_callfunc(L, this, "on_disconnect", status, context_->errstr);
}

void rdsclient::on_reply(redisReply* reply, void* privdata)
{
    int token = (long)privdata;
    luaL_pushfunc(L, this, "on_reply");
    luaL_pushvalue(L, token);
    luaL_pushstatus(L, reply);
    luaL_pushreply(L, reply);
    luaL_safecall(L, 3, 0);
}

int rdsclient::command(lua_State* L)
{
    std::vector<const char*> args;
    std::vector<size_t> lens;

    int top = lua_gettop(L);
    for (int i = 1; i <= top; i++)
    {
        size_t len = 0;
        const char* arg = lua_tolstring(L, i, &len);
        if (arg == nullptr || len == 0)
            return 0;
        args.push_back(arg);
        lens.push_back(len);
    }

    int token = ++last_token_;
    int status = redisAsyncCommandArgv(context_, redis_on_reply, (void*)token, args.size(), args.data(), lens.data());
    if (status != REDIS_OK)
    {
        return 0;
    }

    lua_pushinteger(L, token);
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
        { "on_connect", lua_emptyfunc },
        { "on_disconnect", lua_emptyfunc },
        { "on_reply", lua_emptyfunc },
        { IMPORT_OFUNC(rdsclient, command) },
        { IMPORT_OFUNC(rdsclient, close) },
        { NULL, NULL }
    };
    return libs;
}