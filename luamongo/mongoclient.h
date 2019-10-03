#ifndef _JINJIAZHANG_MONGOCLIENT_H_
#define _JINJIAZHANG_MONGOCLIENT_H_

#include "mongoc/mongoc.h"

class mongoclient
{
public:
    mongoclient(mongoc_client_pool_t* pool);
	~mongoclient();

    int command(const char* cmd);

private:
    mongoc_client_pool_t* pool_;
};

#endif