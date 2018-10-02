#!/bin/sh
#set -x

# build thirdparty
cd thirdparty
if [ ! -d lib ];then
    mkdir lib
fi
if [ ! -d include ];then
    mkdir include
fi
if [ ! -d lua-5.3.5/build ];then
    mkdir lua-5.3.5/build
fi
if [ ! -d curl-7.61.0/build ];then
    mkdir curl-7.61.0/build
fi
if [ ! -d log4cplus/build ];then
    mkdir log4cplus/build
fi

# build lua
cd lua-5.3.5/build
cmake -DCMAKE_BUILD_TYPE=Release ../src
make lua
cp ./liblua.a ../../lib/liblua.a
cd ../..

# build curl
cd curl-7.61.0/build
cmake -DCMAKE_BUILD_TYPE=Release -DCURL_STATICLIB=ON ..
make libcurl
cp ./lib/libcurl.a ../../lib/libcurl.a
cd ../..

# build log4cplus
cd log4cplus/build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DUNICODE=OFF ..
make log4cplusS
cp ./src/liblog4cplusS.a ../../lib/liblog4cplus.a
cd ../..

# build protobuf
cd protobuf/cmake
cmake -DCMAKE_BUILD_TYPE=Release -Dprotobuf_MSVC_STATIC_RUNTIME=OFF ../cmake
make libprotobuf
cp ./libprotobuf.a ../../lib/libprotobuf.a
cd ../..

# copy include
cd include
if [ ! -d lua ];then
	mkdir lua
	cp ../lua-5.3.5/src/lauxlib.h ./lua
	cp ../lua-5.3.5/src/lua.h ./lua
	cp ../lua-5.3.5/src/lua.hpp ./lua
	cp ../lua-5.3.5/src/luaconf.h ./lua
	cp ../lua-5.3.5/src/lualib.h ./lua
fi

if [ ! -d curl ];then
	mkdir curl
	cp -r ../curl-7.61.0/include/curl ./
fi

if [ ! -d log4cplus ];then
	mkdir log4cplus
	cp -r ../log4cplus/include/log4cplus ./
	cp -r ../log4cplus/build/include/log4cplus ./
fi

if [ ! -d google ];then
	mkdir google
	cp -r ../protobuf/src/google ./
fi

# goto root
cd ../..

if [ ! -d build ];then
    mkdir build
fi

cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
cp ./luapp/Release/luapp ../bin/luapp
cd ../..
