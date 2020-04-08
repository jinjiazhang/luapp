#ifndef _JINJIAZHANG_CONSUMER_H_
#define _JINJIAZHANG_CONSUMER_H_

#include "lualib/lobject.h"
#include "rdkafka.h"

class luakafka;
class consumer : public lobject
{
public:
    consumer(lua_State* L, luakafka* kafka);
    ~consumer();

    bool init(std::map<std::string, std::string>& confs, std::string& errmsg);
    int update(int timeout);

    int subscribe(lua_State* L);
    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:
    void consume_message(const rd_kafka_message_t* rkm);

private:
    luakafka* kafka_;
    rd_kafka_t* rk_;
};

#endif