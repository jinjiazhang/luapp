#include "sqlpool.h"
#include "sqlclient.h"
#include "mysqlmgr.h"

#define SQL_METHOD_SELECT   1
#define SQL_METHOD_INSERT   2
#define SQL_METHOD_UPDATE   3
#define SQL_METHOD_DELETE   4
#define SQL_METHOD_CREATE   5
#define SQL_METHOD_EXECUTE  6
#define SQL_METHOD_INCREASE 7

using namespace google::protobuf;

// from protolua
bool decode_message(const Message& message, const Descriptor* descriptor, lua_State* L);
bool encode_message(Message* message, const Descriptor* descriptor, lua_State* L, int index);

sqlpool::sqlpool(lua_State* L) : lobject(L)
{
    last_token_ = 0;
    sqlmgr_ = nullptr;
    run_flag_ = false;
}

sqlpool::~sqlpool()
{
    run_flag_ = false;
    for (auto& thread : threads_)
    {
        thread.join();
    }
}

bool sqlpool::init(mysqlmgr* sqlmgr)
{
    sqlmgr_ = sqlmgr;
    run_flag_ = true;
    return true;
}

int sqlpool::update()
{
    rsp_mutex_.lock();
    auto rsp_queue = std::move(rsp_queue_);
    rsp_mutex_.unlock();

    for (auto task : rsp_queue)
    {
        on_respond(task);
    }    
    return 0;
}

void sqlpool::work_thread(const char* host, const char* user, const char* passwd, const char* db, unsigned int port)
{
    sqlclient* client = new sqlclient();
    client->connect(host, user, passwd, db, port);
    while (run_flag_)
    {
        std::shared_ptr<taskdata> task;
        req_mutex_.lock();
        if (!req_queue_.empty())
        {
            task = req_queue_.front();
            req_queue_.pop_front();
        }
        req_mutex_.unlock();

        if (!task)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }

        do_request(client, task);

        rsp_mutex_.lock();
        rsp_queue_.push_back(task);
        rsp_mutex_.unlock();
    }
    delete client;
}

int sqlpool::connect(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* host = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    const char* user = luaL_getvalue<const char*>(L, 2);
    luaL_checktype(L, 3, LUA_TSTRING);
    const char* passwd = luaL_getvalue<const char*>(L, 3);
    luaL_checktype(L, 4, LUA_TSTRING);
    const char* db = luaL_getvalue<const char*>(L, 4);
    luaL_checktype(L, 5, LUA_TNUMBER);
    unsigned int port = luaL_getvalue<int>(L, 5);

    threads_.push_back(std::thread(&sqlpool::work_thread, this, host, user, passwd, db, port));
    lua_pushboolean(L, true);
    return 1;
}

// pool.sql_select("tbPlayer", "role_id = 10001")
int sqlpool::sql_select(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TSTRING);
    const char* proto = luaL_getvalue<const char*>(L, 1);
    const Descriptor* descriptor = sqlmgr_->find_message(proto);
    if (descriptor == nullptr)
    {
        log_error("sqlpool::sql_select message not found, proto=%s", proto);
        return 0;
    }

    std::shared_ptr<taskdata> task(new taskdata());
    task->token = ++last_token_;
    task->method = SQL_METHOD_SELECT;
    task->content = luaL_getvalue<std::string>(L, 2);
    task->descriptor = descriptor;
    
    req_mutex_.lock();
    req_queue_.push_back(task);
    req_mutex_.unlock();

    lua_pushinteger(L, task->token);
    return 1;
}

// pool.sql_insert("tbPlayer", player)
int sqlpool::sql_insert(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TTABLE);
    const char* proto = luaL_getvalue<const char*>(L, 1);
    const Descriptor* descriptor = sqlmgr_->find_message(proto);
    if (descriptor == nullptr)
    {
        log_error("sqlpool::sql_insert message not found, proto=%s", proto);
        return 0;
    }

    const Message* prototype = factory_.GetPrototype(descriptor);
    std::shared_ptr<Message> message(prototype->New());
    if (!encode_message(message.get(), descriptor, L, 2))
    {
        log_error("sqlpool::sql_insert encode message fail, proto=%s", proto);
        return 0;
    }

    std::shared_ptr<taskdata> task(new taskdata());
    task->token = ++last_token_;
    task->method = SQL_METHOD_INSERT;
    task->message = message;

    req_mutex_.lock();
    req_queue_.push_back(task);
    req_mutex_.unlock();

    lua_pushinteger(L, task->token);
    return 1;
}

// pool.sql_update("tbPlayer", player, "role_id = 10001")
int sqlpool::sql_update(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TTABLE);
    luaL_checktype(L, 3, LUA_TSTRING);
    const char* proto = luaL_getvalue<const char*>(L, 1);
    const Descriptor* descriptor = sqlmgr_->find_message(proto);
    if (descriptor == nullptr)
    {
        log_error("sqlpool::sql_update message not found, proto=%s", proto);
        return 0;
    }

    const Message* prototype = factory_.GetPrototype(descriptor);
    std::shared_ptr<Message> message(prototype->New());
    if (!encode_message(message.get(), descriptor, L, 2))
    {
        log_error("sqlpool::sql_update encode message fail, proto=%s", proto);
        return 0;
    }

    std::shared_ptr<taskdata> task(new taskdata());
    task->token = ++last_token_;
    task->method = SQL_METHOD_UPDATE;
    task->content = luaL_getvalue<std::string>(L, 3);
    task->message = message;

    req_mutex_.lock();
    req_queue_.push_back(task);
    req_mutex_.unlock();

    lua_pushinteger(L, task->token);
    return 1;
}

// pool.sql_delete("tbPlayer", "role_id = 10001")
int sqlpool::sql_delete(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TSTRING);
    const char* proto = luaL_getvalue<const char*>(L, 1);
    const Descriptor* descriptor = sqlmgr_->find_message(proto);
    if (descriptor == nullptr)
    {
        log_error("sqlpool::sql_delete message not found, proto=%s", proto);
        return 0;
    }

    std::shared_ptr<taskdata> task(new taskdata());
    task->token = ++last_token_;
    task->method = SQL_METHOD_DELETE;
    task->content = luaL_getvalue<std::string>(L, 2);
    task->descriptor = descriptor;

    req_mutex_.lock();
    req_queue_.push_back(task);
    req_mutex_.unlock();

    lua_pushinteger(L, task->token);
    return 1;
}

int sqlpool::sql_create(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* proto = luaL_getvalue<const char*>(L, 1);
    const Descriptor* descriptor = sqlmgr_->find_message(proto);
    if (descriptor == nullptr)
    {
        log_error("sqlpool::sql_create message not found, proto=%s", proto);
        return 0;
    }

    std::shared_ptr<taskdata> task(new taskdata());
    task->token = ++last_token_;
    task->method = SQL_METHOD_CREATE;
    task->descriptor = descriptor;

    req_mutex_.lock();
    req_queue_.push_back(task);
    req_mutex_.unlock();

    lua_pushinteger(L, task->token);
    return 1;
}

// pool.sql_execute("delete from tbPlayer where role_id = 10001")
int sqlpool::sql_execute(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    std::shared_ptr<taskdata> task(new taskdata());
    task->token = ++last_token_;
    task->method = SQL_METHOD_EXECUTE;
    task->content = luaL_getvalue<std::string>(L, 1);

    req_mutex_.lock();
    req_queue_.push_back(task);
    req_mutex_.unlock();

    lua_pushinteger(L, task->token);
    return 1;
}

void sqlpool::do_request(sqlclient* client, std::shared_ptr<taskdata> task)
{
    switch (task->method)
    {
    case SQL_METHOD_SELECT:
        task->ret_code = client->sql_select(task->descriptor, task->content, task->results);
        break;
    case SQL_METHOD_INSERT:
        task->ret_code = client->sql_insert(task->message.get());
        break;
    case SQL_METHOD_UPDATE:
        task->ret_code = client->sql_update(task->message.get(), task->content);
        break;
    case SQL_METHOD_DELETE:
        task->ret_code = client->sql_delete(task->descriptor, task->content);
        break;
    case SQL_METHOD_CREATE:
        task->ret_code = client->sql_create(task->descriptor);
        break;
    case SQL_METHOD_EXECUTE:
        task->ret_code = client->sql_execute(task->content);
        break;
    default:
        break;
    }
}

void sqlpool::on_respond(std::shared_ptr<taskdata> task)
{
    switch (task->method)
    {
    case SQL_METHOD_SELECT:
        on_selected(task);
        break;
    case SQL_METHOD_INSERT:
    case SQL_METHOD_UPDATE:
    case SQL_METHOD_DELETE:
    case SQL_METHOD_EXECUTE:
        luaL_callfunc(L, this, "on_respond", task->token, task->ret_code);
        break;
    default:
        break;
    }
}

void sqlpool::on_selected(std::shared_ptr<taskdata> task)
{
    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_respond");
    luaL_pushvalue(L, task->token);
    luaL_pushvalue(L, task->ret_code);
    for (auto message : task->results)
    {
        if (!decode_message(*message, message->GetDescriptor(), L))
        {
            lua_settop(L, top);
            return;
        }
    }
    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
}

EXPORT_OFUNC(sqlpool, connect)
EXPORT_OFUNC(sqlpool, sql_select)
EXPORT_OFUNC(sqlpool, sql_insert)
EXPORT_OFUNC(sqlpool, sql_update)
EXPORT_OFUNC(sqlpool, sql_delete)
EXPORT_OFUNC(sqlpool, sql_create)
EXPORT_OFUNC(sqlpool, sql_execute)
const luaL_Reg* sqlpool::get_libs()
{
    static const luaL_Reg libs[] = {
        { "on_respond", lua_emptyfunc },
        { IMPORT_OFUNC(sqlpool, connect) },
        { IMPORT_OFUNC(sqlpool, sql_select) },
        { IMPORT_OFUNC(sqlpool, sql_insert) },
        { IMPORT_OFUNC(sqlpool, sql_update) },
        { IMPORT_OFUNC(sqlpool, sql_delete) },
        { IMPORT_OFUNC(sqlpool, sql_create) },
        { IMPORT_OFUNC(sqlpool, sql_execute) },
        { NULL, NULL }
    };
    return libs;
}