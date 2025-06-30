CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(ODBC)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("./sqlite" "./include")
add_library(sqlite3 STATIC "./sqlite/sqlite3.c")
add_library(http_utils STATIC "./src/http_utils.cpp")
add_library(sql_utils STATIC "./src/sql_utils.cpp")

add_executable(main "./main.cpp")

set_property(TARGET http_utils PROPERTY CXX_STANDARD 17)
set_property(TARGET main PROPERTY CXX_STANDARD 17)

target_link_libraries(http_utils sql_utils)
target_link_libraries(main sqlite3 http_utils)
