#ifndef _JINJIAZHANG_RTSTRUCT_H_
#define _JINJIAZHANG_RTSTRUCT_H_

typedef unsigned int    svrid_t;

enum rtm_type
{
    invalid = 0,
    reg_svrid,
    call_self,
    call_target,
    call_random,
};

#pragma pack(1)
struct rtm_head
{
    unsigned char msg_type;
};

struct rtm_reg_svrid : rtm_head
{
    svrid_t svrid;
};

struct rtm_call_self : rtm_head
{
    svrid_t srcid;
};

struct rtm_call_target : rtm_head
{
    svrid_t dstid;
};

struct rtm_call_random : rtm_head
{
    svrid_t group;
};

#pragma pack()

#endif