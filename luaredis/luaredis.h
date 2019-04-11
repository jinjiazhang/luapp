#ifndef _JINJIAZHANG_LUAREDIS_H_
#define _JINJIAZHANG_LUAREDIS_H_

#include "lualib/lobject.h"

class luaredis : public lobject
{
public:
	luaredis(lua_State* L);
	~luaredis();

	int connect(const char* ip, int port);

private:

};

#endif