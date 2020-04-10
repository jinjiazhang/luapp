#include "gwtools.h"

// tcp://127.0.0.1:8086?encrypt=1
bool parse_url(std::string url, proxy_param& param)
{
    std::vector<std::string> rs1 = split_string(url, "://");
    TOOLS_CHECK(rs1.size() == 2);

    if (rs1[0] == "tcp")
        param.protocol = protocol_type::tcp;
    else if (rs1[0] == "udp")
        param.protocol = protocol_type::udp;
    else if (rs1[0] == "kcp")
        param.protocol = protocol_type::kcp;
    else
        return false;

    std::vector<std::string> rs2 = split_string(rs1[1], "?");
    std::vector<std::string> rs3 = split_string(rs2[0], ":");
    TOOLS_CHECK(rs3.size() == 2);

    param.ip = rs3[0];
    param.port = atoi(rs3[1].c_str());
    param.encrypt = false;

    std::map<std::string, std::string> opts;
    if (rs2.size() >= 2)
    {
        std::vector<std::string> rs4 = split_string(rs2[1], "&");
        for (std::string& str : rs4)
        {
            std::vector<std::string> rs5 = split_string(str, "=");
            TOOLS_CHECK(rs5.size() == 2);
            opts.insert(std::make_pair(rs5[0], rs5[1]));
        }
    }

    // todo parse options
    return true;
}

std::vector<std::string> split_string(std::string& str, const std::string& delim)
{
    std::vector<std::string> results;
    size_t last = 0;
    size_t index = str.find(delim, last);
    while (index != std::string::npos)
    {
        results.push_back(str.substr(last, index - last));
        last = index + delim.size();
        index = str.find(delim, last);
    }
    if (index - last > 0)
    {
        results.push_back(str.substr(last, index - last));
    }
    return results;
}