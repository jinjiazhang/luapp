set VS_PATH=C:\Program Files (x86)\MSBuild\14.0\Bin\
set PATH=%PATH%;%VS_PATH%

rem build deps
cd dep
if not exist lib (mkdir lib)
if not exist lua-5.3.5/build (mkdir "lua-5.3.5/build")
if not exist log4cplus/build (mkdir "log4cplus/build")

rem build lua
cd lua-5.3.5/build
cmake ..\src
MSBuild.exe ./lua.vcxproj /p:Configuration=Release
copy .\Release\lua.lib ..\..\lib\lua.lib
cd ../..

rem build log4cplus
cd log4cplus/build
cmake -DBUILD_SHARED_LIBS=OFF -DUNICODE=OFF ..
MSBuild.exe ./src/log4cplusS.vcxproj /p:Configuration=Release
copy .\src\Release\log4cplusS.lib ..\..\lib\log4cplus.lib
cd ../..

rem build protobuf
cd protobuf/cmake
cmake -Dprotobuf_MSVC_STATIC_RUNTIME=OFF ..\cmake
MSBuild.exe ./libprotobuf.vcxproj /p:Configuration=Release
copy .\Release\libprotobuf.lib ..\..\lib\protobuf.lib
cd ../..

rem goto root
cd ..


rem copy include
cd include
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

rem goto root
cd ..


if not exist build (mkdir build)
cd build
cmake ..