#include "gwproxy.h"
#include "gwserver.h"
#include "protolua/message.h"

gwproxy::gwproxy(lua_State* L) : lobject(L)
{
    server_ = nullptr;
    encrypt_ = false;
}

gwproxy::~gwproxy()
{

}

bool gwproxy::init(gwserver* server, url_info* args)
{
    server_ = server;
    encrypt_ = args->encrypt;
    return true;
}

int gwproxy::update()
{
    return 0;
}

void gwproxy::start_session(int connid, svrid_t svrid)
{

}

void gwproxy::stop_session(int connid)
{
    connid_cipher_map_.erase(connid);
}

static char output[MESSAGE_BUFFER_SIZE];
void gwproxy::send(int connid, const void* data, int len)
{
    if (encrypt_)
    {
        int outlen = sizeof(output);
        cipher& cipher_ = connid_cipher_map_[connid];
        cipher_.encrypt((const char*)data, len, output, &outlen);
        raw_send(connid, output, outlen);
    }
    else
    {
        raw_send(connid, data, len);
    }
}

void gwproxy::raw_send(int connid, const void* data, int len)
{

}

void gwproxy::send_key(int connid, const char* key, int len)
{
    char temp[AES_KEY_SIZE + 1] = { 0 };
    temp[0] = len;
    if (key != nullptr)
        memcpy(temp + 1, key, len);
    raw_send(connid, temp, sizeof(temp));
}

void gwproxy::on_accept(int connid, int error)
{
    if (encrypt_)
    {
        cipher& cipher_ = connid_cipher_map_[connid];
        cipher_.init();
        send_key(connid, cipher_.get_key(), AES_KEY_SIZE);
    }
    else
    {
        send_key(connid, nullptr, 0);
    }

    server_->reg_connid(connid, this);
    luaL_callfunc(L, this, "on_accept", connid, error);
}

void gwproxy::on_closed(int connid, int error)
{
    luaL_callfunc(L, this, "on_closed", connid, error);
    server_->unreg_connid(connid);

    connid_cipher_map_.erase(connid);
}

void gwproxy::on_package(int connid, char* data, int len)
{
    if (encrypt_)
    {
        int outlen = sizeof(output);
        cipher& cipher_ = connid_cipher_map_[connid];
        cipher_.decrypt((const char*)data, len, output, &outlen);
        raw_package(connid, output, outlen);
    }
    else
    {
        raw_package(connid, data, len);
    }
}

void gwproxy::raw_package(int connid, char* data, int len)
{
    if (server_->is_accepted(connid))
    {
        server_->transmit_data(connid, data, len);
        return;
    }

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