cmake problem:
thirdparty\mongo-c-driver\build\calc_release_version.py
git tag --merged HEAD --list 1.*

build problem:
mv thirdparty/log4cplus/include/log4cplus/config/defines.hxx
mv thirdparty/mongo-c-driver/src/libbson/src/bson/bson-config.h
mv thirdparty/mongo-c-driver/src/libbson/src/bson/bson-version.h
mv thirdparty/mongo-c-driver/src/libmongoc/src/mongoc/mongoc-config.h
mv thirdparty/mongo-c-driver/src/libmongoc/src/mongoc/mongoc-version.h
mv thirdparty/mariadb-connector-c/include/mariadb_version.h