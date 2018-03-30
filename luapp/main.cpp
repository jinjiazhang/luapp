#include "cmdline.h"
#include "luapp.h"

using namespace std;
int main(int argc, char* argv[])
{
    cmdline::parser cmd;
    cmd.add<string>("main-file", 'f', "lua file which process entry.", false, "mail.lua");
    cmd.add("daemon", 'D', "start this process as a daemon.");
    cmd.parse_check(argc, argv);

    luctx* ctx = new luctx;
    memset(ctx, 0, sizeof(ctx));
    ctx->entry = cmd.get<string>("main-file");
    ctx->daemon = cmd.exist("daemon");

    luapp* app = new luapp();
    app->run(ctx);
    delete app;
    return 0;
}