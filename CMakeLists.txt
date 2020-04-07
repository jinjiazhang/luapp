cmake_minimum_required(VERSION 3.1)
project(luapp)


#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
	set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
	set (CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /MT")
	set (CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /MT")

	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MT")
	set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

add_definitions(-DLOG4CPLUS_DISABLE_DLL_RUNTIME_WARNING)

add_subdirectory(luapp)
add_subdirectory(lualib)
add_subdirectory(crypto)
add_subdirectory(gateway)
add_subdirectory(luaredis)
add_subdirectory(luamongo)
add_subdirectory(luamysql)
add_subdirectory(luakafka)
add_subdirectory(proroute)
add_subdirectory(protolua)
add_subdirectory(protonet)
add_subdirectory(thirdparty)

add_dependencies(luapp lualib crypto gateway luaredis luamysql proroute protolua protonet 
	lua log4cplusS libprotobuf libcurl hiredis mariadbclient mongoc_static bson_static rdkafka)