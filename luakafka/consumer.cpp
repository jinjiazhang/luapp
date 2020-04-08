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
    return 0;
}

void consumer::consume_message(const rd_kafka_message_t* rkm)
{

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