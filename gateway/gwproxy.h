#ifndef _JINJIAZHANG_GWPROXY_H_
#define _JINJIAZHANG_GWPROXY_H_

#include "lualib/lobject.h"
#include "gwstruct.h"
#include "gwtools.h"
#include "cipher.h"
#include <unordered_map>

class gwserver;
class gwproxy : public lobject
{
public:
    gwproxy(lua_State* L);
    virtual ~gwproxy();

    virtual bool init(gwserver* server, proxy_param param);
    virtual int  update();
    virtual void start_session(int connid, svrid_t svrid);
    virtual void stop_session(int connid);
    virtual void send(int connid, const void* data, int len);
    virtual void raw_send(int connid, const void* data, int len);
    virtual void send_key(int connid, const char* key, int len);

    virtual void on_accept(int connid, int error);
    virtual void on_closed(int connid, int error);
    virtual void on_package(int connid, char* data, int len);
    virtual void raw_package(int connid, char* data, int len);

protected:
    typedef std::unordered_map<connid_t, cipher> connid_cipher_map;

    gwserver* server_;
    bool encrypt_;
    connid_cipher_map connid_cipher_map_;
};

#endif