cmake_minimum_required(VERSION 3.1)
project(thirdparty)

option (CURL_STATICLIB "curl static lib" ON)
option (BUILD_SHARED_LIBS "log4cplus build shared lib" OFF)
option (UNICODE "log4cplus use unicode" OFF)
option (CMAKE_USE_OPENSSL "Use OpenSSL code. Experimental" OFF)

# http://slproweb.com/products/Win32OpenSSL.html
# http://slproweb.com/download/Win32OpenSSL-1_1_1b.exe
find_package(OpenSSL)
IF (OpenSSL_FOUND)
	set(CMAKE_USE_OPENSSL ON)
ENDIF()

add_subdirectory(lua-5.3.5 EXCLUDE_FROM_ALL)
add_subdirectory(curl EXCLUDE_FROM_ALL)
add_subdirectory(protobuf/cmake EXCLUDE_FROM_ALL)
add_subdirectory(hiredis EXCLUDE_FROM_ALL)
add_subdirectory(mariadb-connector-c EXCLUDE_FROM_ALL)
add_subdirectory(log4cplus EXCLUDE_FROM_ALL)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET lua PROPERTY FOLDER "thirdparty")
set_property(TARGET libcurl PROPERTY FOLDER "thirdparty")
set_property(TARGET log4cplusS loggingserver PROPERTY FOLDER "thirdparty")
set_property(TARGET libprotobuf libprotobuf-lite PROPERTY FOLDER "thirdparty")
set_property(TARGET hiredis PROPERTY FOLDER "thirdparty")
set_property(TARGET mariadbclient PROPERTY FOLDER "thirdparty")