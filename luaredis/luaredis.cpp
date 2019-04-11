#include "luaredis.h"
#include "hiredis.h"

luaredis::luaredis(lua_State* L) : lobject(L)
{
}

luaredis::~luaredis()
{
}

int luaredis::connect(const char* ip, int port)
{
	redisContext* context = redisConnect(ip, port);
	if (context == nullptr)
	{
		return -1;
	}
	return context->err;
}
