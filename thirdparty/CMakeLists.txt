cmake_minimum_required(VERSION 3.1)
project(thirdparty)

option (CURL_STATICLIB "curl static lib" ON)
option (BUILD_TESTING "curl build testing" OFF)
option (ENABLE_MANUAL "curl enable manual" OFF)
option (BUILD_CURL_EXE "curl build exe" OFF)
option (BUILD_SHARED_LIBS "log4cplus build shared lib" OFF)
option (LOG4CPLUS_BUILD_TESTING "log4cplus build testing" OFF)
option (UNICODE "log4cplus use unicode" OFF)
option (protobuf_BUILD_TESTS "protobuf build tests" OFF)
option (protobuf_BUILD_PROTOC_BINARIES "protobuf build binaries" OFF)
option (CMAKE_USE_OPENSSL "Use OpenSSL code. Experimental" OFF)

# http://slproweb.com/products/Win32OpenSSL.html
# http://slproweb.com/download/Win32OpenSSL-1_1_1b.exe
find_package(OpenSSL)
IF (OpenSSL_FOUND)
	set(CMAKE_USE_OPENSSL ON)
ENDIF()

add_subdirectory(lua-5.3.5)
add_subdirectory(curl)
add_subdirectory(log4cplus)
add_subdirectory(protobuf/cmake)
add_subdirectory(hiredis)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET lua PROPERTY FOLDER "thirdparty")
set_property(TARGET libcurl PROPERTY FOLDER "thirdparty")
set_property(TARGET log4cplusS loggingserver PROPERTY FOLDER "thirdparty")
set_property(TARGET libprotobuf libprotobuf-lite PROPERTY FOLDER "thirdparty")
set_property(TARGET hiredis PROPERTY FOLDER "thirdparty")