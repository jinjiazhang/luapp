#include "mongopool.h"
#include "mongoclient.h"
#include "luamongo.h"

#define MONGO_METHOD_CONNECT  0
#define MONGO_METHOD_SELECT   1
#define MONGO_METHOD_INSERT   2
#define MONGO_METHOD_UPDATE   3
#define MONGO_METHOD_DELETE   4
#define MONGO_METHOD_CREATE   5
#define MONGO_METHOD_EXECUTE  6
#define MONGO_METHOD_INCREASE 7

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
    mongoclient* client = new mongoclient(pool);
    int ret_code = client->command("ping");
    if (ret_code != 0)
    {
        log_error("mongoclient::command fail, code(%d)", ret_code);
        std::shared_ptr<taskdata> task(new taskdata());
        task->token = 0;
        task->method = MONGO_METHOD_CONNECT;
        task->ret_code = ret_code;
        rsp_mutex_.lock();
        rsp_queue_.push_back(task);
        rsp_mutex_.unlock();
        return;
    }

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

void mongopool::do_request(mongoclient* client, std::shared_ptr<taskdata> task)
{

}

void mongopool::on_respond(std::shared_ptr<taskdata> task)
{

}

const luaL_Reg* mongopool::get_libs()
{
    static const luaL_Reg libs[] = {
        { "on_respond", lua_emptyfunc },
        { NULL, NULL }
    };
    return libs;
}