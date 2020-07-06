cmake_minimum_required(VERSION 3.0)
project(QBitor VERSION 0.1.0 LANGUAGES C)

configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)
include_directories(
    include
    src/core
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(qbitor src/client/main.c src/client/commands.c)
add_executable(qbitord src/daemon/main.c src/daemon/loop.c)

add_subdirectory(src/core)

find_library(cwalk cwalk lib)
find_library(sqlite sqlite3 /usr/lib/x86_64-linux-gnu)

target_link_libraries(qbitord core ${cwalk} ${sqlite})
target_link_libraries(qbitor ${cwalk})