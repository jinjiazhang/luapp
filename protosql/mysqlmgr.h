#ifndef _JINJIAZHANG_MYSQLMGR_H_
#define _JINJIAZHANG_MYSQLMGR_H_

#include "lualib/lobject.h"
#include "google/protobuf/message.h"
#include "google/protobuf/compiler/importer.h"
#include "google/protobuf/dynamic_message.h"

class mysqlmgr : public lobject
{
public:
    mysqlmgr(lua_State* L);
    ~mysqlmgr();

    int parse(lua_State* L);
    int create_pool(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:
    google::protobuf::compiler::Importer importer_;
    google::protobuf::compiler::DiskSourceTree source_tree_;    
};

#endif