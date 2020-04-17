#ifndef _JINJIAZHANG_GWTOOLS_H_
#define _JINJIAZHANG_GWTOOLS_H_

#include <vector>
#include <string>
#include <map>

enum class protocol_type
{
    tcp = 1,
    udp,
    kcp,
};

struct url_info
{
    protocol_type protocol;
    char ip[16];
    int port;
    bool encrypt;
};

#define TOOLS_CHECK(exp) { if(!(exp)) return false; }

bool parse_url(std::string url, url_info* args);
std::vector<std::string> split_string(std::string& str, const std::string& delim);

#endif