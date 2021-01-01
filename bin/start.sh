#!/bin/sh

./luapp --id=1.1.1.1 --main-file=router/main.lua --log-conf=config/router-linux.properties --daemon
./luapp --id=1.1.2.1 --main-file=dbagent/main.lua --log-conf=config/dbagent-linux.properties --daemon
./luapp --id=1.1.3.1 --main-file=listsvr/main.lua --log-conf=config/listsvr-linux.properties --daemon
./luapp --id=1.1.4.1 --main-file=gamesvr/main.lua --log-conf=config/gamesvr-linux.properties --daemon
./luapp --id=1.1.5.1 --main-file=gateway/main.lua --log-conf=config/gateway-linux.properties --daemon
./luapp --id=1.1.6.1 --main-file=lobby/main.lua --log-conf=config/lobby-linux.properties --daemon
# ./luapp --id=1.1.9.1 --main-file=robot/main.lua --log-conf=config/robot-linux.properties

