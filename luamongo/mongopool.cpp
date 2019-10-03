#include "mongopool.h"
#include "luamongo.h"
#include "bsonutil.h"

#define MONGO_METHOD_CONNECT  0
#define MONGO_METHOD_COMMAND  1
#define MONGO_METHOD_INSERT   2
#define MONGO_METHOD_FIND     3
#define MONGO_METHOD_UPDATE   4
#define MONGO_METHOD_REPLACE  5
#define MONGO_METHOD_DELETE   6

mongopool::mongopool(lua_State* L, luamongo* mongo) : lobject(L)
{
    last_token_ = 0;
    mongo_ = mongo;
    uri_ = nullptr;
    pool_ = nullptr;
    run_flag_ = false;
}

mongopool::~mongopool()
{
    run_flag_ = false;
    for (auto& thread : threads_)
    {
        thread.join();
    }

    if (pool_)
    {
        mongoc_client_pool_destroy(pool_);
        pool_ = nullptr;
    }

    if (uri_)
    {
        mongoc_uri_destroy(uri_);
        uri_ = nullptr;
    }
}

bool mongopool::init(const char* url, int num)
{
    bson_error_t error;
    uri_ = mongoc_uri_new_with_error(url, &error);
    if (uri_ == nullptr)
    {
        lua_pushstring(L, error.message);
        return 1;
    }

    pool_ = mongoc_client_pool_new(uri_);
    mongoc_client_pool_set_error_api(pool_, 2);

    run_flag_ = true;
    for (int i = 0; i < num; i++) {
        threads_.push_back(std::thread(&mongopool::work_thread, this, pool_));
    }
    return true;
}

int mongopool::update()
{
    rsp_mutex_.lock();
    auto rsp_queue = std::move(rsp_queue_);
    rsp_mutex_.unlock();

    int count = 0;
    for (auto task : rsp_queue)
    {
        on_respond(task);
        count++;
    }    
    return count;
}

void mongopool::work_thread(void* data)
{
    mongoc_client_pool_t* pool = (mongoc_client_pool_t*)data;
    mongoc_client_t* client = mongoc_client_pool_pop(pool);

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
    mongoc_client_pool_push(pool, client);
}

// pool.mongo_command("db_name", {ping = 1})
int mongopool::mongo_command(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TTABLE);

    std::shared_ptr<taskdata> task(new taskdata());
    task->token = ++last_token_;
    task->method = MONGO_METHOD_COMMAND;
    task->db_name = lua_tostring(L, 1);
    task->param1 = luaL_tobson(L, 2);

    req_mutex_.lock();
    req_queue_.push_back(task);
    req_mutex_.unlock();

    lua_pushinteger(L, task->token);
    return 1;
}

int mongopool::mongo_insert(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TSTRING);
    luaL_checktype(L, 3, LUA_TTABLE);

    std::shared_ptr<taskdata> task(new taskdata());
    task->token = ++last_token_;
    task->method = MONGO_METHOD_INSERT;
    task->db_name = lua_tostring(L, 1);
    task->coll_name = lua_tostring(L, 2);
    task->param1 = luaL_tobson(L, 3);
    if (lua_istable(L, 4)) {
        task->param2 = luaL_tobson(L, 4);
    }

    req_mutex_.lock();
    req_queue_.push_back(task);
    req_mutex_.unlock();

    lua_pushinteger(L, task->token);
    return 1;
}

int mongopool::mongo_find(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TSTRING);
    luaL_checktype(L, 3, LUA_TTABLE);

    std::shared_ptr<taskdata> task(new taskdata());
    task->token = ++last_token_;
    task->method = MONGO_METHOD_FIND;
    task->db_name = lua_tostring(L, 1);
    task->coll_name = lua_tostring(L, 2);
    task->param1 = luaL_tobson(L, 3);
    if (lua_istable(L, 4)) {
        task->param2 = luaL_tobson(L, 4);
    }

    req_mutex_.lock();
    req_queue_.push_back(task);
    req_mutex_.unlock();

    lua_pushinteger(L, task->token);
    return 1;
}

int mongopool::mongo_update(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TSTRING);
    luaL_checktype(L, 3, LUA_TTABLE);
    luaL_checktype(L, 4, LUA_TTABLE);

    std::shared_ptr<taskdata> task(new taskdata());
    task->token = ++last_token_;
    task->method = MONGO_METHOD_UPDATE;
    task->db_name = lua_tostring(L, 1);
    task->coll_name = lua_tostring(L, 2);
    task->param1 = luaL_tobson(L, 3);
    task->param2 = luaL_tobson(L, 4);
    if (lua_istable(L, 5)) {
        task->param3 = luaL_tobson(L, 5);
    }

    req_mutex_.lock();
    req_queue_.push_back(task);
    req_mutex_.unlock();

    lua_pushinteger(L, task->token);
    return 1;
}

int mongopool::mongo_replace(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TSTRING);
    luaL_checktype(L, 3, LUA_TTABLE);
    luaL_checktype(L, 4, LUA_TTABLE);

    std::shared_ptr<taskdata> task(new taskdata());
    task->token = ++last_token_;
    task->method = MONGO_METHOD_REPLACE;
    task->db_name = lua_tostring(L, 1);
    task->coll_name = lua_tostring(L, 2);
    task->param1 = luaL_tobson(L, 3);
    task->param2 = luaL_tobson(L, 4);
    if (lua_istable(L, 5)) {
        task->param3 = luaL_tobson(L, 5);
    }

    req_mutex_.lock();
    req_queue_.push_back(task);
    req_mutex_.unlock();

    lua_pushinteger(L, task->token);
    return 1;
}

int mongopool::mongo_delete(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TSTRING);
    luaL_checktype(L, 3, LUA_TTABLE);

    std::shared_ptr<taskdata> task(new taskdata());
    task->token = ++last_token_;
    task->method = MONGO_METHOD_DELETE;
    task->db_name = lua_tostring(L, 1);
    task->coll_name = lua_tostring(L, 2);
    task->param1 = luaL_tobson(L, 3);
    if (lua_istable(L, 4)) {
        task->param2 = luaL_tobson(L, 4);
    }

    req_mutex_.lock();
    req_queue_.push_back(task);
    req_mutex_.unlock();

    lua_pushinteger(L, task->token);
    return 1;
}

void mongopool::do_request(mongoc_client_t* client, std::shared_ptr<taskdata> task)
{
    mongoc_collection_t* collection;
    mongoc_cursor_t* cursor;
    const bson_t* document;
    switch (task->method)
    {
    case MONGO_METHOD_COMMAND:
        task->retval = mongoc_client_command_simple(client, task->db_name.c_str(), task->param1, nullptr, &task->reply, &task->error);
        break;
    case MONGO_METHOD_INSERT:
        collection = mongoc_client_get_collection(client, task->db_name.c_str(), task->coll_name.c_str());
        task->retval = mongoc_collection_insert_one(collection, task->param1, task->param2, &task->reply, &task->error);
        mongoc_collection_destroy(collection);
        break;
    case MONGO_METHOD_FIND:
        collection = mongoc_client_get_collection(client, task->db_name.c_str(), task->coll_name.c_str());
        cursor = mongoc_collection_find_with_opts(collection, task->param1, task->param2, nullptr);
        while (mongoc_cursor_next(cursor, &document)) {
            task->results.push_back(bson_copy(document));
        }
        mongoc_cursor_destroy(cursor);
        mongoc_collection_destroy(collection);
        break;
    case MONGO_METHOD_UPDATE:
        collection = mongoc_client_get_collection(client, task->db_name.c_str(), task->coll_name.c_str());
        task->retval = mongoc_collection_update_one(collection, task->param1, task->param2, task->param3, &task->reply, &task->error);
        mongoc_collection_destroy(collection);
    case MONGO_METHOD_REPLACE:
        collection = mongoc_client_get_collection(client, task->db_name.c_str(), task->coll_name.c_str());
        task->retval = mongoc_collection_replace_one(collection, task->param1, task->param2, task->param3, &task->reply, &task->error);
        mongoc_collection_destroy(collection);
    case MONGO_METHOD_DELETE:
        collection = mongoc_client_get_collection(client, task->db_name.c_str(), task->coll_name.c_str());
        task->retval = mongoc_collection_delete_one(collection, task->param1, task->param2, &task->reply, &task->error);
        mongoc_collection_destroy(collection);
    default:
        break;
    }
}

void mongopool::on_respond(std::shared_ptr<taskdata> task)
{
    switch (task->method)
    {
    case MONGO_METHOD_FIND:
        luaL_pushfunc(L, this, "on_respond");
        luaL_pushvalue(L, task->token);
        task->retval ? lua_pushnil(L) : luaL_pushvalue(L, task->error.message);
        lua_newtable(L);
        for (int i = 0; i < task->results.size(); i++)
        {
            luaL_pushbson(L, task->results[i]);
            lua_seti(L, -2, i + 1);
        }
        luaL_safecall(L, 3, 0);
        break;
    case MONGO_METHOD_COMMAND:
    case MONGO_METHOD_INSERT:
    case MONGO_METHOD_UPDATE:
        luaL_pushfunc(L, this, "on_respond");
        luaL_pushvalue(L, task->token);
        task->retval ? lua_pushnil(L) : luaL_pushvalue(L, task->error.message);
        luaL_pushbson(L, &task->reply);
        luaL_safecall(L, 3, 0);
        break;
    default:
        break;
    }
}

EXPORT_OFUNC(mongopool, mongo_command)
EXPORT_OFUNC(mongopool, mongo_insert)
EXPORT_OFUNC(mongopool, mongo_find)
EXPORT_OFUNC(mongopool, mongo_update)
EXPORT_OFUNC(mongopool, mongo_replace)
EXPORT_OFUNC(mongopool, mongo_delete)
const luaL_Reg* mongopool::get_libs()
{
    static const luaL_Reg libs[] = {
        { "on_respond", lua_emptyfunc },
        { IMPORT_OFUNC(mongopool, mongo_command) },
        { IMPORT_OFUNC(mongopool, mongo_insert) },
        { IMPORT_OFUNC(mongopool, mongo_find) },
        { IMPORT_OFUNC(mongopool, mongo_update) },
        { IMPORT_OFUNC(mongopool, mongo_replace) },
        { IMPORT_OFUNC(mongopool, mongo_delete) },
        { NULL, NULL }
    };
    return libs;
}