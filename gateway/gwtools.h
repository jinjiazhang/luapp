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

struct proxy_param
{
    protocol_type protocol;
    std::string ip;
    int port;
    bool encrypt;
};

#define TOOLS_CHECK(exp) { if(!(exp)) return false; }

bool parse_url(std::string url, proxy_param& param);
std::vector<std::string> split_string(std::string& str, const std::string& delim);

#endif