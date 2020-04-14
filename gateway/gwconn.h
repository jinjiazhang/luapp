#ifndef _JINJIAZHANG_GWCONN_H_
#define _JINJIAZHANG_GWCONN_H_

#include "lualib/lobject.h"
#include "protonet/inetwork.h"
#include "gwtools.h"
#include "cipher.h"

class gateway;
class gwconn : public lobject, public imanager
{
public:
    gwconn(lua_State* L);
    virtual ~gwconn();

    int number();
    virtual bool init(gateway* gate, url_info* args);
    int call(lua_State* L);
    void close(lua_State* L);
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int connid, int error);
    virtual void on_closed(int connid, int error);
    virtual void on_package(int connid, char* data, int len);
    virtual void raw_package(int connid, char* data, int len);

    virtual void send(const void* data, int len);
    virtual void recv_key(const char* key, int len);

protected:
    typedef std::vector<std::string> send_cache;

    gateway* gateway_;
    inetwork* network_;
    cipher cipher_;
    send_cache send_cache_;

    int number_;
    bool encrypt_;
    bool key_recv_;
};

#endif