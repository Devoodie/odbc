CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(ODBC)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("./sqlite")
add_library(sqlite3 STATIC "./sqlite/sqlite3.c")

add_executable(main "./main.cpp")

target_link_libraries(main sqlite3)
