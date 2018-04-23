#include "cmdline.h"
#include "luapp.h"

using namespace std;
int main(int argc, char* argv[])
{
    cmdline::parser cmd;
    cmd.add<string>("main-file", 'm', "lua file which process entry.", false, "./main.lua");
    cmd.add<int>("tick-freq", 't', "call tick function interval.", false, 20);
    cmd.add<int>("idle-sleep", 'i', "sleep duration when idle.", false, 10);
    cmd.add("daemon", 'D', "start this process as a daemon.");
    cmd.parse_check(argc, argv);

    luctx* ctx = new luctx();
    memset(ctx, 0, sizeof(ctx));
    ctx->entry = cmd.get<string>("main-file").c_str();
    ctx->tick_freq = cmd.get<int>("tick-freq");
    ctx->tick_invl = ctx->tick_freq > 0 ? (1000 / ctx->tick_freq) : 0x7FFFFFFF;
    ctx->idle_sleep = cmd.get<int>("idle-sleep");
    ctx->daemon = cmd.exist("daemon");

    luapp* app = new luapp();
    app->run(ctx);
    delete app;
    return 0;
}