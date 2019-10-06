#ifndef _JINJIAZHANG_RTSTRUCT_H_
#define _JINJIAZHANG_RTSTRUCT_H_

typedef unsigned int            svrid_t;
typedef unsigned int            group_t;
typedef unsigned long long      roleid_t;

enum rtm_type
{
    invalid = 0,
    reg_svrid,
    reg_roleid,
    unreg_roleid,
    remote_call,
    transmit_call,
    forward_svrid,
    forward_roleid,
    forward_group,
    forward_random,
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

struct rtm_reg_roleid : rtm_head
{
    roleid_t roleid;
    group_t group;
};

struct rtm_unreg_roleid : rtm_head
{
    roleid_t roleid;
    group_t group;
};

struct rtm_remote_call : rtm_head
{
    svrid_t srcid;
};

struct rtm_transmit_call : rtm_head
{
    roleid_t roleid;
    svrid_t srcid;
};

struct rtm_forward_svrid : rtm_head
{
    svrid_t dstid;
};

struct rtm_forward_roleid : rtm_head
{
    roleid_t roleid;
    group_t group;
};

struct rtm_forward_group : rtm_head
{
    group_t group;
};

struct rtm_forward_random : rtm_head
{
    group_t group;
};

#pragma pack()

#endif