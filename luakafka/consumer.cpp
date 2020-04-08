#include "consumer.h"
#include "luakafka.h"

consumer::consumer(lua_State* L, luakafka* kafka) : lobject(L)
{
    kafka_ = nullptr;
    rk_ = nullptr;
}

consumer::~consumer()
{
    if (rk_)
    {
        rd_kafka_consumer_close(rk_);
        rd_kafka_destroy(rk_);
        rk_ = nullptr;
    }
}

static void rk_log_cb(const rd_kafka_t* rk, int level, const char* fac, const char* buf)
{
    switch (level)
    {
    case 7:
        log_debug("%s %s", rd_kafka_name(rk), buf);
        break;
    case 6:
    case 5:
        log_info("%s %s", rd_kafka_name(rk), buf);
        break;
    case 4:
        log_warn("%s %s", rd_kafka_name(rk), buf);
        break;
    case 3:
        log_error("%s %s", rd_kafka_name(rk), buf);
        break;
    default:
        log_fatal("%s %s", rd_kafka_name(rk), buf);
        break;
    }
}

bool consumer::init(std::map<std::string, std::string>& confs, std::string& errmsg)
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
    rd_kafka_conf_set_log_cb(conf, rk_log_cb);
    rk_ = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
    if (rk_ == nullptr)
    {
        errmsg = errstr;
        return false;
    }

    rd_kafka_poll_set_consumer(rk_);
    return true;
}

int consumer::update(int timeout)
{
    int count = 0;
    rd_kafka_message_t* rkm;
    while (rkm = rd_kafka_consumer_poll(rk_, timeout))
    {
        consume_message(rkm);
        rd_kafka_message_destroy(rkm);
        count++;
    }
    return count;
}

int consumer::subscribe(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    int topic_cnt = lua_gettop(L);
    rd_kafka_topic_partition_list_t* subscription = rd_kafka_topic_partition_list_new(topic_cnt);
    for (int i = 0; i < topic_cnt; i++)
    {
        luaL_checktype(L, i + 1, LUA_TSTRING);
        const char* topic = lua_tostring(L, i + 1);
        rd_kafka_topic_partition_list_add(subscription, topic, RD_KAFKA_PARTITION_UA);
    }

    rd_kafka_resp_err_t ret = rd_kafka_subscribe(rk_, subscription);
    if (ret != 0) {
        lua_pushboolean(L, false);
        lua_pushstring(L, rd_kafka_err2str(ret));
        rd_kafka_topic_partition_list_destroy(subscription);
        return 2;
    }

    lua_pushboolean(L, true);
    rd_kafka_topic_partition_list_destroy(subscription);
    return 1;
}

void consumer::consume_message(const rd_kafka_message_t* rkm)
{
    if (rkm->err) 
    {
        luaL_pushfunc(L, this, "on_error");
        lua_pushstring(L, rd_kafka_topic_name(rkm->rkt));
        lua_pushlstring(L, (const char*)rkm->payload, rkm->len);
        lua_pushlstring(L, (const char*)rkm->key, rkm->key_len);
        lua_pushstring(L, rd_kafka_message_errstr(rkm));
        luaL_safecall(L, 4, 0);
    }
    else
    {
        luaL_pushfunc(L, this, "on_consume");
        lua_pushstring(L, rd_kafka_topic_name(rkm->rkt));
        lua_pushlstring(L, (const char*)rkm->payload, rkm->len);
        lua_pushlstring(L, (const char*)rkm->key, rkm->key_len);
        lua_pushinteger(L, rkm->offset);
        luaL_safecall(L, 4, 0);
    }
}

int consumer::close(lua_State* L)
{
    kafka_->destory_consumer(this);
    return 0;
}

EXPORT_OFUNC(consumer, subscribe)
EXPORT_OFUNC(consumer, close)
const luaL_Reg* consumer::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(consumer, subscribe) },
        { IMPORT_OFUNC(consumer, close) },
        { NULL, NULL }
    };
    return libs;
}