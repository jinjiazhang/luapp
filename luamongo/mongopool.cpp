#include "mongopool.h"
#include "luamongo.h"
#include "bsonutil.h"

#define MONGO_METHOD_CONNECT  0
#define MONGO_METHOD_COMMAND  1
#define MONGO_METHOD_INSERT   2

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
    task->command = luaL_tobson(L, 2);

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
    task->document = luaL_tobson(L, 3);

    req_mutex_.lock();
    req_queue_.push_back(task);
    req_mutex_.unlock();

    lua_pushinteger(L, task->token);
    return 1;
}

void mongopool::do_request(mongoc_client_t* client, std::shared_ptr<taskdata> task)
{
    mongoc_collection_t *collection;
    switch (task->method)
    {
    case MONGO_METHOD_COMMAND:
        task->retval = mongoc_client_command_simple(client, task->db_name.c_str(), task->command, nullptr, &task->reply, &task->error);
        break;
    case MONGO_METHOD_INSERT:
        collection = mongoc_client_get_collection(client, task->db_name.c_str(), task->coll_name.c_str());
        task->retval = mongoc_collection_insert_one(collection, task->document, nullptr, &task->reply, &task->error);
        mongoc_collection_destroy(collection);
        break;
    default:
        break;
    }
}

void mongopool::on_respond(std::shared_ptr<taskdata> task)
{
    switch (task->method)
    {
    case MONGO_METHOD_COMMAND:
    case MONGO_METHOD_INSERT:
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
const luaL_Reg* mongopool::get_libs()
{
    static const luaL_Reg libs[] = {
        { "on_respond", lua_emptyfunc },
        { IMPORT_OFUNC(mongopool, mongo_command) },
        { IMPORT_OFUNC(mongopool, mongo_insert) },
        { NULL, NULL }
    };
    return libs;
}