set VS_PATH=C:\Program Files (x86)\MSBuild\14.0\Bin\
set PATH=%PATH%;%VS_PATH%

rem build thirdparty
cd thirdparty
if not exist lib (mkdir lib)
if not exist lua-5.3.5/build (mkdir "lua-5.3.5/build")
if not exist curl-7.61.0/build (mkdir "curl-7.61.0/build")
if not exist log4cplus/build (mkdir "log4cplus/build")
if not exist protobuf/cmake/build (mkdir "protobuf/cmake/build")

rem build lua
cd lua-5.3.5/build
cmake ..\src
MSBuild.exe ./lua.vcxproj /p:Configuration=Release
copy .\Release\lua.lib ..\..\lib\lua.lib
cd ../..

rem build curl
cd curl-7.61.0/build
cmake -DCURL_STATICLIB=ON ..
MSBuild.exe ./lib/libcurl.vcxproj /p:Configuration=Release
copy .\lib\Release\libcurl.lib ..\..\lib\curl.lib
cd ../..

rem build log4cplus
cd log4cplus/build
cmake -DBUILD_SHARED_LIBS=OFF -DUNICODE=OFF ..
MSBuild.exe ./src/log4cplusS.vcxproj /p:Configuration=Release
copy .\src\Release\log4cplusS.lib ..\..\lib\log4cplus.lib
cd ../..

rem build protobuf
cd protobuf/cmake/build
cmake -Dprotobuf_MSVC_STATIC_RUNTIME=OFF ..
MSBuild.exe ./libprotobuf.vcxproj /p:Configuration=Release
copy .\Release\libprotobuf.lib ..\..\..\lib\protobuf.lib
cd ../../..


rem build luapp
cd ..
if not exist build (mkdir build)
cd build
cmake ..
MSBuild.exe ./ALL_BUILD.vcxproj /p:Configuration=Release
copy .\luapp\Release\luapp.exe ..\bin\luapp.exe
cd ../..