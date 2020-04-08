#include "producer.h"
#include "luakafka.h"

producer::producer(lua_State* L, luakafka* kafka) : lobject(L)
{
    kafka_ = kafka;
    rk_ = nullptr;
}

producer::~producer()
{
    if (rk_)
    {
        rd_kafka_flush(rk_, 3000);
        rd_kafka_destroy(rk_);
        rk_ = nullptr;
    }
}

static void dr_msg_cb(rd_kafka_t* rk, const rd_kafka_message_t* rkm, void* opaque)
{
    producer* obj = static_cast<producer*>(opaque);
    obj->on_dr_msg_cb(rk, rkm);
}

bool producer::init(std::map<std::string, std::string>& confs, std::string& errmsg)
{
    char errstr[512];
    rd_kafka_conf_t* conf = rd_kafka_conf_new();
    std::map<std::string, std::string>::iterator it = confs.begin();
    for (; it != confs.end(); ++it)
    {
        rd_kafka_conf_res_t ret = rd_kafka_conf_set(conf, it->first.c_str(), it->second.c_str(), errstr, sizeof(errstr));
        if (ret != RD_KAFKA_CONF_OK)
        {
            errmsg = errstr;
            return false;
        }
    }

    rd_kafka_conf_set_opaque(conf, this);
    rd_kafka_conf_set_dr_msg_cb(conf, &dr_msg_cb);
    rk_ = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
    if (rk_ == nullptr) 
    {
        errmsg = errstr;
        return false;
    }
    return true;
}

int producer::update(int timeout)
{
    return rd_kafka_poll(rk_, timeout);
}

// producer.produce(topic, data, key)
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
        RD_KAFKA_V_OPAQUE(NULL),
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

void producer::on_dr_msg_cb(rd_kafka_t* rk, const rd_kafka_message_t* rkm)
{
    assert(rk == rk_);
    if (rkm->err)
    {
        int top = lua_gettop(L);
        luaL_pushfunc(L, this, "on_error");
        lua_pushstring(L, rd_kafka_topic_name(rkm->rkt));
        lua_pushlstring(L, (const char*)rkm->payload, rkm->len);
        lua_pushlstring(L, (const char*)rkm->key, rkm->key_len);
        lua_pushstring(L, rd_kafka_err2str(rkm->err));
        luaL_safecall(L, 4, 0);
    }
}

int producer::close(lua_State* L)
{
    kafka_->destory_producer(this);
    return 0;
}

EXPORT_OFUNC(producer, produce)
EXPORT_OFUNC(producer, close)
const luaL_Reg* producer::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(producer, produce) },
        { IMPORT_OFUNC(producer, close) },
        { NULL, NULL }
    };
    return libs;
}