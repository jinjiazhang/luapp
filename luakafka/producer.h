#ifndef _JINJIAZHANG_PRODUCER_H_
#define _JINJIAZHANG_PRODUCER_H_

#include "lualib/lobject.h"
#include "rdkafka.h"

class luakafka;
class producer : public lobject
{
public:
    producer(lua_State* L, luakafka* kafka);
    ~producer();

    bool init(std::map<std::string, std::string>& confs, std::string& errmsg);
    int update(int timeout);

    int produce(lua_State* L);
    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

    void on_dr_msg_cb(rd_kafka_t* rk, const rd_kafka_message_t* rkmessage);

private:
    luakafka* kafka_;
    rd_kafka_t* rk_;
};

#endif