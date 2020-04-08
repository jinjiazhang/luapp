#ifndef _JINJIAZHANG_PRODUCER_H_
#define _JINJIAZHANG_PRODUCER_H_

#include "lualib/lobject.h"
#include "rdkafka.h"

class producer : public lobject
{
public:
    producer(lua_State* L);
    ~producer();

    bool init(std::map<std::string, std::string>& confs);

    int produce(lua_State* L);
    int poll(lua_State* L);
    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:
    rd_kafka_t* rk_;
};

#endif