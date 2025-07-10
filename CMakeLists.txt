CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(ODBC)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("./sqlite" "./include")
add_library(sqlite3 STATIC "./sqlite/sqlite3.c")
add_library(http_utils STATIC "./src/http_utils.cpp")
add_library(sql_utils STATIC "./src/sql_utils.cpp")
add_library(argon2 STATIC IMPORTED)
SET_TARGET_PROPERTIES(argon2 PROPERTIES IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libargon2.a)

add_executable(main "./main.cpp")

set_property(TARGET http_utils PROPERTY CXX_STANDARD 17)
set_property(TARGET main PROPERTY CXX_STANDARD 17)

target_link_libraries(sql_utils argon2)
target_link_libraries(http_utils sql_utils)
target_link_libraries(main sqlite3 http_utils argon2)
