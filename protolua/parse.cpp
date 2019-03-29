#include "protolua.h"

using namespace google::protobuf;
using namespace google::protobuf::compiler;

bool proto_parse(const char* file, lua_State* L)
{
    const FileDescriptor* parsed_file = g_importer.Import(file);
    if (parsed_file == NULL) {
        return false;
    }
    return true;
}