#!/bin/sh
#set -x

# yum install libstdc++-devel.x86_64 libstdc++.x86_64 compat-libstdc++-33.x86_64 libstdc++-static.x86_64 
# yum install openssl openssl-devel
# yum install libcurl-devel.x86_64

# build thirdparty
cd thirdparty
if [ ! -d lib ];then
    mkdir lib
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
if [ ! -d protobuf/cmake/build ];then
    mkdir protobuf/cmake/build
fi

# build lua
cd lua-5.3.5/build
cmake -DCMAKE_BUILD_TYPE=Release ../src
make lua
cp ./liblua.a ../../lib/liblua.a
cd ../..

# build curl
## cd curl-7.61.0/build
## cmake -DCMAKE_BUILD_TYPE=Release -DCURL_STATICLIB=ON ..
## make libcurl
## cp ./lib/libcurl.a ../../lib/libcurl.a
## cd ../..

# build log4cplus
cd log4cplus/build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DUNICODE=OFF ..
make log4cplusS
cp ./src/liblog4cplusS.a ../../lib/liblog4cplus.a
cd ../..

# build protobuf
cd protobuf/cmake/build
cmake -DCMAKE_BUILD_TYPE=Release -Dprotobuf_MSVC_STATIC_RUNTIME=OFF ..
make libprotobuf
cp ./libprotobuf.a ../../../lib/libprotobuf.a
cd ../../..


# build luapp
cd ..
if [ ! -d build ];then
    mkdir build
fi
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
cp ./luapp/luapp ../bin/luapp
cd ../..
