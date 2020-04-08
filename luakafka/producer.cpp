#include "producer.h"

producer::producer(lua_State* L) : lobject(L)
{
    rk_ = nullptr;
}

producer::~producer()
{

}

static void dr_msg_cb(rd_kafka_t* rk, const rd_kafka_message_t* rkmessage, void* opaque)
{

}

bool producer::init(std::map<std::string, std::string>& confs)
{
    char errstr[512];
    rd_kafka_conf_t* conf = rd_kafka_conf_new();
    std::map<std::string, std::string>::iterator it = confs.begin();
    for (; it != confs.end(); ++it)
    {
        rd_kafka_conf_res_t ret = rd_kafka_conf_set(conf, it->first.c_str(), it->second.c_str(), errstr, sizeof(errstr));
        if (ret != RD_KAFKA_CONF_OK)
        {
            return false;
        }
    }

    rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);
    rk_ = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
    if (rk_ == nullptr) 
    {
        return false;
    }
    return true;
}

// producer.produce()
int producer::produce(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TSTRING);
    const char* topic = lua_tostring(L, 1);
    size_t len, key_len;
    const char* payload = lua_tolstring(L, 2, &len);
    const char* key = lua_tolstring(L, 3, &key_len);

    rd_kafka_resp_err_t ret = rd_kafka_producev( rk_,
        RD_KAFKA_V_TOPIC(topic),
        RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
        RD_KAFKA_V_VALUE(payload, len),
        RD_KAFKA_V_KEY(key, key_len),
        RD_KAFKA_V_OPAQUE(this),
        RD_KAFKA_V_END);

    if (ret != 0)
    {
        lua_pushboolean(L, false);
        lua_pushstring(L, rd_kafka_err2str(ret));
        return 2;
    }

    lua_pushboolean(L, true);
    return 1;
}

int producer::poll(lua_State* L)
{
    return 0;
}

int producer::close(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(producer, produce)
EXPORT_OFUNC(producer, poll)
EXPORT_OFUNC(producer, close)
const luaL_Reg* producer::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(producer, produce) },
        { IMPORT_OFUNC(producer, poll) },
        { IMPORT_OFUNC(producer, close) },
        { NULL, NULL }
    };
    return libs;
}