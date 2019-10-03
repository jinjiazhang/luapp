#ifndef _JINJIAZHANG_MONGOPOOL_H_
#define _JINJIAZHANG_MONGOPOOL_H_

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
        int ret_code = 0;
    };

public:
    mongopool(lua_State* L, luamongo* mongo);
    ~mongopool();

    bool init(const char* url, int num);
    int  update();
    void work_thread(void* data);

    virtual const luaL_Reg* get_libs();

private:
    void do_request(mongoclient* client, std::shared_ptr<taskdata> task);
    void on_respond(std::shared_ptr<taskdata> task);

private:
    luamongo* mongo_;
    mongoc_uri_t* uri_;
    mongoc_client_pool_t* pool_;
    int last_token_;
    std::atomic_bool run_flag_;
    std::vector<std::thread> threads_;

    std::mutex req_mutex_;
    std::list<std::shared_ptr<taskdata>> req_queue_;

    std::mutex rsp_mutex_;
    std::list<std::shared_ptr<taskdata>> rsp_queue_;
};

#endif