#include "lhttp.h"

lhttp::lhttp(lua_State* L) : lobject(L)
{
    http_ = new http();
}

lhttp::~lhttp()
{
    delete http_;
}

void lhttp::respond(int token, int code, const char* data)
{
    luaL_callfunc(L, this, "respond", token, code, data);
}

int lhttp::update()
{
    return http_->update();
}

int lhttp::get(const char* url)
{
    return http_->get(this, url);
}

int lhttp::post(const char* url, const char* data)
{
    return http_->post(this, url, data);
}

EXPORT_OFUNC(lhttp, get)
EXPORT_OFUNC(lhttp, post)
const luaL_Reg* lhttp::get_libs()
{
    static const luaL_Reg libs[] = {
        { "respond", lua_emptyfunc },
        { IMPORT_OFUNC(lhttp, get) },
        { IMPORT_OFUNC(lhttp, post) },
        { NULL, NULL }
    };
    return libs;
}