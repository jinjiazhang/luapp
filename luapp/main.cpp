#include "cmdline.h"
#include "luapp.h"
#include "plat.h"

int main(int argc, char* argv[])
{
    cmdline::parser cmd;
    cmd.add<std::string>("id", 'i', "server identity like '1.2.3.4'.", false, "id=1.1.9.1");
    cmd.add<std::string>("main-file", 'm', "lua file which process entry.", false, "robot/main.lua");
    cmd.add<std::string>("log-conf", 'l', "log4cplus properties file.", false, "robot/log.properties");
    cmd.add<int>("tick-freq", 't', "call tick function interval.", false, 20);
    cmd.add<int>("idle-sleep", 's', "sleep duration when idle.", false, 10);
    cmd.add("daemon", 'D', "start this process as a daemon.");
    cmd.parse_check(argc, argv);

    luctx* ctx = new luctx();
    memset(ctx, 0, sizeof(luctx));
    ctx->svrid = svrid_stoi(cmd.get<std::string>("id"));
    ctx->entry = cmd.get<std::string>("main-file").c_str();
    ctx->log_conf = cmd.get<std::string>("log-conf").c_str();
    ctx->tick_freq = cmd.get<int>("tick-freq");
    ctx->tick_invl = ctx->tick_freq > 0 ? (1000 / ctx->tick_freq) : 0x7FFFFFFF;
    ctx->idle_sleep = cmd.get<int>("idle-sleep");
    ctx->daemon = cmd.exist("daemon");

    log4cplus::initialize();
    PropertyConfigurator::doConfigure(ctx->log_conf);
    lua_State* L = luaL_newstate();
    luapp* app = new luapp(L);
    app->run(ctx);
    delete app;
    lua_close(L);
    return 0;
}