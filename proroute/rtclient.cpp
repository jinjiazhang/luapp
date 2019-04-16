#include "rtclient.h"
#include "routermgr.h"
#include "protolua/protolua.h"

rtclient::rtclient(lua_State* L, svrid_t svrid) : lobject(L)
{
    svrid_ = svrid;
    router_ = 0;
}

rtclient::~rtclient()
{
    
}

int rtclient::number()
{
    return number_;
}

bool rtclient::init(routermgr* manager, int number)
{
    network_ = manager->network();
    manager_ = manager;
    number_ = number;
    return true;
}

void rtclient::close()
{

}

void rtclient::on_accept(int number, int error)
{
    assert(number == number_);
    if (error != 0)
    {
        luaL_callfunc(L, this, "on_accept", router_, error);
        return;
    }

    rtm_reg_svrid msg;
    msg.msg_type = rtm_type::reg_svrid;
    msg.svrid = svrid_;
    network_->send(number_, &msg, sizeof(msg));
}

void rtclient::on_closed(int number, int error)
{
    assert(number == number_);
    luaL_callfunc(L, this, "on_closed", router_, error);
}

void rtclient::on_package(int number, char* data, int len)
{
    assert(number == number_);
    if (len < sizeof(rtm_head))
    {
        log_error("rtclient::on_package length =%d invalid", len);
        return;
    }

    rtm_head* head = (rtm_head*)data;
    switch ((rtm_type)head->msg_type)
    {
    case rtm_type::reg_svrid:
        on_reg_svrid(data, len);
        break;
    case rtm_type::call_self:
        on_call_self(data, len);
        break;
    default:
        log_error("rtclient::on_package msg_type =%d invalid", head->msg_type);
        break;
    }
}

void rtclient::on_reg_svrid(char* data, int len)
{
    rtm_reg_svrid* msg = (rtm_reg_svrid*)data;
    router_ = msg->svrid;
    luaL_callfunc(L, this, "on_accept", router_, 0);
}

void rtclient::on_call_self(char* data, int len)
{
    rtm_call_self* msg = (rtm_call_self*)data;
    data += sizeof(rtm_call_self);
    len -= sizeof(rtm_call_self);

    std::string proto = data;
    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_message");
    luaL_pushvalue(L, msg->srcid);
    luaL_pushvalue(L, proto);

    const char* input = data + proto.size() + 1;
    size_t size = len - proto.size() - 1;
    if (!proto_unpack(proto.c_str(), L, input, size))
    {
        lua_settop(L, top);
        return;
    }

    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
}

int rtclient::call_target(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    svrid_t dstid = luaL_getvalue<int>(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    std::string proto = luaL_getvalue<std::string>(L, 2);

    static char buffer[64 * 1024];
    strcpy(buffer, proto.c_str());

    int top = lua_gettop(L);
    char* output = buffer + proto.size() + 1;
    size_t size = sizeof(buffer) - proto.size() - 1;
    if (!proto_pack(proto.c_str(), L, 3, top, output, &size))
    {
        lua_pushboolean(L, false);
        return 1;
    }

    rtm_call_target head;
    head.msg_type = rtm_type::call_target;
    head.dstid = dstid;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { &buffer, (int)(proto.size() + 1 + size) };
    network_->sendv(number_, bufs, 2);
    lua_pushboolean(L, true);
    return 1;
}

EXPORT_OFUNC(rtclient, call_target)
const luaL_Reg* rtclient::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(rtclient, call_target) },
        { NULL, NULL }
    };
    return libs;
}