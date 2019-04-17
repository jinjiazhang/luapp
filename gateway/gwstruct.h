#ifndef _JINJIAZHANG_GWSTRUCT_H_
#define _JINJIAZHANG_GWSTRUCT_H_

typedef unsigned int            svrid_t;
typedef unsigned long long      roleid_t;

enum gwm_type
{
    invalid = 0,
    reg_svrid,
};

#pragma pack(1)
struct gwm_head
{
    unsigned char msg_type;
};

struct rtm_reg_svrid : gwm_head
{
    svrid_t svrid;
};

#pragma pack()

#endif