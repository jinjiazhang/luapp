#ifndef _JINJIAZHANG_GWSTRUCT_H_
#define _JINJIAZHANG_GWSTRUCT_H_

typedef unsigned int            svrid_t;
typedef unsigned long long      roleid_t;

enum gwm_type
{
    invalid = 0,
    reg_svrid,
    remote_call,
    session_start,
    session_stop,
    start_session,
    stop_session,
    transmit_data,
    broadcast_data,
};

#pragma pack(1)
struct gwm_head
{
    unsigned char msg_type;
};

struct gwm_reg_svrid : gwm_head
{
    svrid_t svrid;
};

struct gwm_remote_call : gwm_head
{
    svrid_t srcid;
};

struct gwm_session_start : gwm_head
{

};

struct gwm_session_stop : gwm_head
{

};

struct gwm_start_session : gwm_head
{

};

struct gwm_stop_session : gwm_head
{

};

struct gwm_transmit_data : gwm_head
{

};

struct gwm_broadcast_data : gwm_head
{

};

#pragma pack()

#endif