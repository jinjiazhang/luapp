#ifndef _JINJIAZHANG_SQLPOOL_H_
#define _JINJIAZHANG_SQLPOOL_H_

#include <list>
#include "lualib/lobject.h"
#include "google/protobuf/dynamic_message.h"


class mysqlmgr;
class sqlclient;
class sqlpool : public lobject
{
public:
    struct taskdata
    {
        int token = 0;
        int method = 0;
        int ret_code = 0;
        std::string content;
        const google::protobuf::Descriptor* descriptor = nullptr;
        std::shared_ptr<google::protobuf::Message> message;
        std::vector<std::shared_ptr<google::protobuf::Message>> results;
    };

public:
    sqlpool(lua_State* L);
    ~sqlpool();

    bool init(mysqlmgr* sqlmgr);
    int  update();
    void work_thread(const char* host, const char* user, const char* passwd, const char* db, unsigned int port);

    int connect(lua_State* L);
    int sql_select(lua_State* L);
    int sql_insert(lua_State* L);
    int sql_update(lua_State* L);
    int sql_delete(lua_State* L);
    int sql_execute(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:
    void do_request(sqlclient* client, std::shared_ptr<taskdata> task);
    void on_respond(std::shared_ptr<taskdata> task);

private:
    mysqlmgr* sqlmgr_;
    int last_token_;
    std::atomic_bool run_flag_;
    std::vector<std::thread> threads_;

    std::mutex req_mutex_;
    std::list<std::shared_ptr<taskdata>> req_queue_;

    std::mutex rsp_mutex_;
    std::list<std::shared_ptr<taskdata>> rsp_queue_;
};

#endif