#ifndef _JINJIAZHANG_PARAMBUF_H_
#define _JINJIAZHANG_PARAMBUF_H_

#include "mysql.h"
#include "google/protobuf/message.h"
#include <vector>

class parambuf
{
public:
    parambuf(int length);
    ~parambuf();

    int parpare(const google::protobuf::Message* message, std::vector<MYSQL_BIND>& binds);

private:
    int length_;
    char* buffer_;
    char* current_;
};

#endif