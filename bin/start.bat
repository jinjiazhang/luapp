rem start.sh

start luapp.exe --id=1.1.1.1 --name=router --main-file=router/main.lua --log-conf=log4cplus-win.properties
start luapp.exe --id=1.1.2.1 --name=dbagent --main-file=dbagent/main.lua --log-conf=log4cplus-win.properties
start luapp.exe --id=1.1.3.1 --name=roomsvr --main-file=roomsvr/main.lua --log-conf=log4cplus-win.properties
start luapp.exe --id=1.1.4.1 --name=gamesvr --main-file=gamesvr/main.lua --log-conf=log4cplus-win.properties
start luapp.exe --id=1.1.5.1 --name=gateway --main-file=gateway/main.lua --log-conf=log4cplus-win.properties
start luapp.exe --id=1.1.6.1 --name=lobby --main-file=lobby/main.lua --log-conf=log4cplus-win.properties
rem start luapp.exe --id=1.1.9.1 --name=robot --main-file=robot/main.lua --log-conf=config/log4cplus-win.properties