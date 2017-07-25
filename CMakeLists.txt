cmake_minimum_required(VERSION 3.7)
project(tinyhttp)

set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/local/lib)
link_directories(/usr/local/lib)


set(SOURCE_FILES
        server/handle.c
        server/handle.h
        server/helper.c
        server/helper.h
        server/main.c
        server/mysocket.c
        server/mysocket.h
        server/sig.c
        server/sig.h
        server/log.h)

add_executable(tinyhttp ${SOURCE_FILES})
target_link_libraries(tinyhttp event)
