#ifndef _JINJIAZHANG_LUAKAFKA_H_
#define _JINJIAZHANG_LUAKAFKA_H_

#include "lualib/lobject.h"

class producer;
class consumer;
class luakafka : public lobject
{
public:
    luakafka(lua_State* L);
    ~luakafka();

    int update();
    int create_producer(lua_State* L);
    int create_consumer(lua_State* L);
    void destory_producer(producer* obj);
    void destory_consumer(consumer* obj);

    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:
    std::vector<producer*> producers_;
    std::vector<consumer*> consumers_;
};

#endif