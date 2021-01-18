#include "cmdline.h"
#include "luapp.h"
#include "plat.h"

void replace_tstring(log4cplus::tstring& value, log4cplus::tstring src, log4cplus::tstring dst)
{
    std::string::size_type pos = 0;
    std::string::size_type srclen = src.size();
    std::string::size_type dstlen = dst.size();

    while ((pos = value.find(src, pos)) != std::string::npos)
    {
        value.replace(pos, srclen, dst);
        pos += dstlen;
    }
}

void replace_log_env_var(log4cplus::helpers::Properties* props, luctx* ctx)
{
    std::vector<log4cplus::tstring> names = props->propertyNames();
    for (size_t i = 0; i < names.size(); i++)
    {
        log4cplus::tstring value = props->getProperty(names[i]);
        replace_tstring(value, "${id}", svrid_itos(ctx->svrid));
        replace_tstring(value, "${name}", ctx->name);
        props->setProperty(names[i], value);
    }
}

int main(int argc, char* argv[])
{
    cmdline::parser cmd;
    cmd.add<std::string>("id", 'i', "server id like '1.2.3.4'.", false, "1.1.9.1");
    cmd.add<std::string>("name", 'n', "server name like 'gamesvr'.", false, "robot");
    cmd.add<std::string>("main-file", 'm', "lua file which process entry.", false, "robot/main.lua");
    cmd.add<std::string>("log-conf", 'l', "log4cplus properties file.", false, "log4cplus.properties");
    cmd.add<int>("tick-freq", 't', "call tick function interval.", false, 20);
    cmd.add<int>("idle-sleep", 's', "sleep duration when idle.", false, 10);
    cmd.add("daemon", 'D', "start this process as a daemon.");
    cmd.parse_check(argc, argv);

    luctx* ctx = new luctx();
    memset(ctx, 0, sizeof(luctx));
    ctx->svrid = svrid_stoi(cmd.get<std::string>("id"));
    ctx->name = cmd.get<std::string>("name").c_str();
    ctx->entry = cmd.get<std::string>("main-file").c_str();
    ctx->log_conf = cmd.get<std::string>("log-conf").c_str();
    ctx->tick_freq = cmd.get<int>("tick-freq");
    ctx->tick_invl = ctx->tick_freq > 0 ? (1000 / ctx->tick_freq) : 0x7FFFFFFF;
    ctx->idle_sleep = cmd.get<int>("idle-sleep");
    ctx->daemon = cmd.exist("daemon");
    set_title(ctx->svrid, ctx->name);

    log4cplus::initialize();
    log4cplus::helpers::Properties props(ctx->log_conf);
    replace_log_env_var(&props, ctx);
    PropertyConfigurator conf(props);
    conf.configure();

    lua_State* L = luaL_newstate();
    luapp* app = new luapp(L);
    app->run(ctx);
    delete app;
    lua_close(L);
    return 0;
}