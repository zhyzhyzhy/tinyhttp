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
        server/log.h
        server/threadpool.c
        server/threadpool.h
        server/util.c
        server/util.h
        server/subreactor.c
        server/subreactor.h
        server/config.c
        server/config.h
        server/mempool.c
        server/mempool.h
        server/parser.c
        server/parser.h)

#set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")
add_executable(tinyhttp ${SOURCE_FILES})
target_link_libraries(tinyhttp event)
