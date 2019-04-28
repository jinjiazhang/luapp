#include "rdsclient.h"
#include "luaredis.h"
#include "async.h"
#include "protonet/network.h"
#include "protolog/protolog.h"

#define REDIS_METHOD_SELECT   1
#define REDIS_METHOD_INSERT   2
#define REDIS_METHOD_UPDATE   3
#define REDIS_METHOD_DELETE   4
#define REDIS_METHOD_COMMAND  5
#define REDIS_METHOD_INCREASE 6

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
    taskdata* task = (taskdata*)privdata;
    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_reply");
    luaL_pushvalue(L, task->token);

    switch (task->method)
    {
    case REDIS_METHOD_SELECT:
        replybuf_.push_select(L, reply);
        break;
    case REDIS_METHOD_INSERT:
        replybuf_.push_insert(L, reply);
        break;
    case REDIS_METHOD_UPDATE:
        replybuf_.push_update(L, reply);
        break;
    case REDIS_METHOD_DELETE:
        replybuf_.push_delete(L, reply);
        break;
    case REDIS_METHOD_COMMAND:
        replybuf_.push_command(L, reply);
        break;
    case REDIS_METHOD_INCREASE:
        replybuf_.push_increase(L, reply);
        break;
    default:
        break;
    }

    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
    delete task;
}

int rdsclient::rds_select(lua_State* L)
{
    std::vector<const char*> args;
    std::vector<size_t> lens;
    int ret = argsbuf_.make_select(L, args, lens);
    if (ret != 0)
    {
        log_error("rdsclient::rds_select make select fail, ret=%d", ret);
        return 0;
    }

    taskdata* task = new taskdata();
    task->token = ++last_token_;
    task->method = REDIS_METHOD_SELECT;
    int status = redisAsyncCommandArgv(context_, redis_on_reply, task, args.size(), args.data(), lens.data());
    if (status != REDIS_OK)
    {
        log_error("rdsclient::rds_select async command fail, status=%d", status);
        return 0;
    }

    lua_pushinteger(L, task->token);
    return 1;
}

int rdsclient::rds_insert(lua_State* L)
{
    std::vector<const char*> args;
    std::vector<size_t> lens;
    int ret = argsbuf_.make_insert(L, args, lens);
    if (ret != 0)
    {
        log_error("rdsclient::rds_insert make insert fail, ret=%d", ret);
        return 0;
    }

    taskdata* task = new taskdata();
    task->token = ++last_token_;
    task->method = REDIS_METHOD_INSERT;
    int status = redisAsyncCommandArgv(context_, redis_on_reply, task, args.size(), args.data(), lens.data());
    if (status != REDIS_OK)
    {
        log_error("rdsclient::rds_insert async command fail, status=%d", status);
        return 0;
    }

    lua_pushinteger(L, task->token);
    return 1;
}

int rdsclient::rds_update(lua_State* L)
{
    std::vector<const char*> args;
    std::vector<size_t> lens;
    int ret = argsbuf_.make_update(L, args, lens);
    if (ret != 0)
    {
        log_error("rdsclient::rds_update make update fail, ret=%d", ret);
        return 0;
    }

    taskdata* task = new taskdata();
    task->token = ++last_token_;
    task->method = REDIS_METHOD_UPDATE;
    int status = redisAsyncCommandArgv(context_, redis_on_reply, task, args.size(), args.data(), lens.data());
    if (status != REDIS_OK)
    {
        log_error("rdsclient::rds_update async command fail, status=%d", status);
        return 0;
    }

    lua_pushinteger(L, task->token);
    return 1;
}

int rdsclient::rds_delete(lua_State* L)
{
    std::vector<const char*> args;
    std::vector<size_t> lens;
    int ret = argsbuf_.make_delete(L, args, lens);
    if (ret != 0)
    {
        log_error("rdsclient::rds_delete make delete fail, ret=%d", ret);
        return 0;
    }

    taskdata* task = new taskdata();
    task->token = ++last_token_;
    task->method = REDIS_METHOD_DELETE;
    int status = redisAsyncCommandArgv(context_, redis_on_reply, task, args.size(), args.data(), lens.data());
    if (status != REDIS_OK)
    {
        log_error("rdsclient::rds_delete async command fail, status=%d", status);
        return 0;
    }

    lua_pushinteger(L, task->token);
    return 1;
}

int rdsclient::rds_command(lua_State* L)
{
    std::vector<const char*> args;
    std::vector<size_t> lens;
    int ret = argsbuf_.make_command(L, args, lens);
    if (ret != 0)
    {
        log_error("rdsclient::rds_command make command fail, ret=%d", ret);
        return 0;
    }

    taskdata* task = new taskdata();
    task->token = ++last_token_;
    task->method = REDIS_METHOD_COMMAND;
    int status = redisAsyncCommandArgv(context_, redis_on_reply, task, args.size(), args.data(), lens.data());
    if (status != REDIS_OK)
    {
        log_error("rdsclient::rds_command async command fail, status=%d", status);
        return 0;
    }

    lua_pushinteger(L, task->token);
    return 1;
}

int rdsclient::rds_increase(lua_State* L)
{
    std::vector<const char*> args;
    std::vector<size_t> lens;
    int ret = argsbuf_.make_command(L, args, lens);
    if (ret != 0)
    {
        log_error("rdsclient::rds_increase make increase fail, ret=%d", ret);
        return 0;
    }

    taskdata* task = new taskdata();
    task->token = ++last_token_;
    task->method = REDIS_METHOD_INCREASE;
    int status = redisAsyncCommandArgv(context_, redis_on_reply, task, args.size(), args.data(), lens.data());
    if (status != REDIS_OK)
    {
        log_error("rdsclient::rds_increase async command fail, status=%d", status);
        return 0;
    }

    lua_pushinteger(L, task->token);
    return 1;
}

int rdsclient::close(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(rdsclient, rds_select)
EXPORT_OFUNC(rdsclient, rds_insert)
EXPORT_OFUNC(rdsclient, rds_update)
EXPORT_OFUNC(rdsclient, rds_delete)
EXPORT_OFUNC(rdsclient, rds_command)
EXPORT_OFUNC(rdsclient, rds_increase)
EXPORT_OFUNC(rdsclient, close)
const luaL_Reg* rdsclient::get_libs()
{
    static const luaL_Reg libs[] = {
        { "on_connect", lua_emptyfunc },
        { "on_disconnect", lua_emptyfunc },
        { "on_reply", lua_emptyfunc },
        { IMPORT_OFUNC(rdsclient, rds_select) },
        { IMPORT_OFUNC(rdsclient, rds_insert) },
        { IMPORT_OFUNC(rdsclient, rds_update) },
        { IMPORT_OFUNC(rdsclient, rds_delete) },
        { IMPORT_OFUNC(rdsclient, rds_command) },
        { IMPORT_OFUNC(rdsclient, rds_increase) },
        { IMPORT_OFUNC(rdsclient, close) },
        { NULL, NULL }
    };
    return libs;
}