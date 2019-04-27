#include "sqlpool.h"
#include "sqlclient.h"
#include "mysqlmgr.h"

using namespace google::protobuf;
#define SQL_METHOD_SELECT   1
#define SQL_METHOD_INSERT   2
#define SQL_METHOD_UPDATE   3
#define SQL_METHOD_DELETE   4
#define SQL_METHOD_EXECUTE  5

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
    auto rsp_queue = std::move(req_queue_);
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

// pool.select("user", "id = 'abc'")
int sqlpool::sql_select(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TSTRING);
    const char* proto = luaL_getvalue<const char*>(L, 1);
    const Descriptor* descriptor = sqlmgr_->find_message(proto);
    if (descriptor == nullptr)
    {
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

int sqlpool::sql_insert(lua_State* L)
{
    return 0;
}

int sqlpool::sql_update(lua_State* L)
{
    return 0;
}

int sqlpool::sql_delete(lua_State* L)
{
    return 0;
}

int sqlpool::sql_execute(lua_State* L)
{
    return 0;
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
    case SQL_METHOD_EXECUTE:
        task->ret_code = client->sql_execute(task->content);
        break;
    default:
        break;
    }
}

void sqlpool::on_respond(std::shared_ptr<taskdata> task)
{
    luaL_callfunc(L, "respond", task->token, task->ret_code);
}

EXPORT_OFUNC(sqlpool, connect)
EXPORT_OFUNC(sqlpool, sql_select)
EXPORT_OFUNC(sqlpool, sql_insert)
EXPORT_OFUNC(sqlpool, sql_update)
EXPORT_OFUNC(sqlpool, sql_delete)
EXPORT_OFUNC(sqlpool, sql_execute)
const luaL_Reg* sqlpool::get_libs()
{
    static const luaL_Reg libs[] = {
        { "respond", lua_emptyfunc },
        { IMPORT_OFUNC(sqlpool, sql_select) },
        { IMPORT_OFUNC(sqlpool, sql_insert) },
        { IMPORT_OFUNC(sqlpool, sql_update) },
        { IMPORT_OFUNC(sqlpool, sql_delete) },
        { IMPORT_OFUNC(sqlpool, connect) },
        { IMPORT_OFUNC(sqlpool, sql_execute) },
        { NULL, NULL }
    };
    return libs;
}