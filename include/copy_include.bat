if not exist lua (
	mkdir lua
	copy ..\dep\lua-5.3.5\src\lauxlib.h .\lua
	copy ..\dep\lua-5.3.5\src\lua.h .\lua
	copy ..\dep\lua-5.3.5\src\lua.hpp .\lua
	copy ..\dep\lua-5.3.5\src\luaconf.h .\lua
	copy ..\dep\lua-5.3.5\src\lualib.h .\lua
)

if not exist curl (
	mkdir curl
	xcopy /E ..\dep\curl-7.61.0\include\curl .\curl
)

if not exist log4cplus (
	mkdir log4cplus
	xcopy  /E ..\dep\log4cplus\include\log4cplus .\log4cplus
)

if not exist google (
	mkdir google
	xcopy  /E ..\dep\protobuf\src\google .\google
)