#ifndef _JINJIAZHANG_MONGOPOOL_H_
#define _JINJIAZHANG_MONGOPOOL_H_

#define BSON_STATIC
#define MONGOC_STATIC
#pragma warning (disable:4316)

#include <list>
#include "lualib/lobject.h"
#include "mongoc/mongoc.h"

class luamongo;
class mongoclient;
class mongopool : public lobject
{
public:
    struct taskdata
    {
        int token = 0;
        int method = 0;
        bool retval = false;
        std::string db_name;
        std::string coll_name;

        bson_t* bson1 = nullptr;
        bson_t* bson2 = nullptr;
        bson_t* bson3 = nullptr;
        std::vector<const bson_t*> results;
        bson_t reply;
        bson_error_t error;
    };

public:
    mongopool(lua_State* L, luamongo* mongo);
    ~mongopool();

    bool init(const char* url, const char* dbname, int num);
    int  update();
    void work_thread(void* data);

    int mongo_command(lua_State* L);
    int mongo_insert(lua_State* L);
    int mongo_find(lua_State* L);
    int mongo_find_many(lua_State* L);
    int mongo_find_and_modify(lua_State* L);
    int mongo_update(lua_State* L);
    int mongo_replace(lua_State* L);
    int mongo_delete(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:
    void do_request(mongoc_client_t* client, std::shared_ptr<taskdata> task);
    void on_respond(std::shared_ptr<taskdata> task);

private:
    luamongo* mongo_;
    mongoc_uri_t* uri_;
    mongoc_client_pool_t* pool_;
    std::string dbname_;

    int last_token_;
    std::atomic_bool run_flag_;
    std::vector<std::thread> threads_;

    std::mutex req_mutex_;
    std::list<std::shared_ptr<taskdata>> req_queue_;

    std::mutex rsp_mutex_;
    std::list<std::shared_ptr<taskdata>> rsp_queue_;
};

#endif
