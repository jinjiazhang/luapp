
set VS_PATH=C:\Program Files (x86)\MSBuild\14.0\Bin\
set PATH=%PATH%;%VS_PATH%

git submodule init
git submodule update
if not exist lib (mkdir lib)

cd log4cplus/msvc14
git submodule init
git submodule update
MSBuild.exe ./log4cplusS.vcxproj /p:Configuration=Release
copy .\Win32\bin.Release\log4cplusS.lib ..\..\lib\log4cplus.lib
cd ../..

cd protobuf/cmake
git submodule init
git submodule update
cmake -Dprotobuf_MSVC_STATIC_RUNTIME=OFF .
MSBuild.exe ./libprotobuf.vcxproj /p:Configuration=Release
copy .\Release\libprotobuf.lib ..\..\lib\protobuf.lib
cd ../..

cd lua-5.3.5/src
cmake .
MSBuild.exe ./lua.vcxproj /p:Configuration=Release
copy .\Release\lua.lib ..\..\lib\lua.lib
cd ../..