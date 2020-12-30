#include "gwconn.h"
#include "gateway.h"
#include "protolua/message.h"

gwconn::gwconn(lua_State* L) : lobject(L)
{
    gateway_ = nullptr;
    network_ = nullptr;
    number_ = 0;
    encrypt_ = false;
    key_recv_ = false;
}

gwconn::~gwconn()
{

}

int gwconn::number()
{
    return number_;
}

bool gwconn::init(gateway* gate, url_info* args)
{
    gateway_ = gate;
    network_ = gateway_->network();

    number_ = network_->connect(this, args->ip, args->port);
    if (number_ <= 0)
    {
        return false;
    }
    return true;
}

int gwconn::call(lua_State* L)
{
    int top = lua_gettop(L);
    size_t msg_len = sizeof(msg_buf);
    if (!message_pack(L, 1, top, msg_buf, &msg_len))
    {
        return 0;
    }

    send(msg_buf, (int)msg_len);
    lua_pushboolean(L, true);
    return 1;
}

void gwconn::close(lua_State* L)
{
    network_->close(number_);
}

void gwconn::on_accept(int connid, int error)
{
    if (error != 0)
    {
        luaL_callfunc(L, this, "on_accept", connid, error);
        return;
    }
}

void gwconn::on_closed(int connid, int error)
{
    luaL_callfunc(L, this, "on_closed", connid, error);
}

static char output[MESSAGE_BUFFER_SIZE];
void gwconn::send(const void* data, int len)
{
    if (!key_recv_)
    {
        send_cache_.push_back(std::string((const char*)data, len));
        return;
    }

    if (encrypt_)
    {
        int outlen = sizeof(output);
        cipher_.encrypt((const char*)data, len, output, &outlen);
        network_->send(number_, output, outlen);
    }
    else
    {
        network_->send(number_, data, len);
    }
}

void gwconn::recv_key(const char* data, int len)
{
    encrypt_ = data[0];
    if (encrypt_)
    {
        cipher_.init(data + 1, len - 1);
    }
}

void gwconn::on_package(int connid, char* data, int len)
{
    if (!key_recv_)
    {
        recv_key(data, len);
        key_recv_ = true;

        for (std::string& cache : send_cache_)
        {
            send(cache.data(), cache.size());
        }
        luaL_callfunc(L, this, "on_accept", connid, 0);
        return;
    }

    if (encrypt_)
    {
        int outlen = sizeof(output);
        cipher_.decrypt((const char*)data, len, output, &outlen);
        raw_package(connid, output, outlen);
    }
    else
    {
        raw_package(connid, data, len);
    }
}

void gwconn::raw_package(int connid, char* data, int len)
{
    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_message");
    luaL_pushvalue(L, connid);

    if (!message_unpack(L, data, len))
    {
        lua_settop(L, top);
        return;
    }
    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
}

EXPORT_OFUNC(gwconn, number)
EXPORT_OFUNC(gwconn, call)
EXPORT_OFUNC(gwconn, close)
const luaL_Reg* gwconn::get_libs()
{
    static const luaL_Reg libs[] = {
        { "number", OFUNC(gwconn, number) },
        { "call", OFUNC(gwconn, call) },
        { "close", OFUNC(gwconn, close) },
        { NULL, NULL }
    };
    return libs;
}