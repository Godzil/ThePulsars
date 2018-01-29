cmake_minimum_required(VERSION 3.0)

project(PulsarServer)

set(TARGET server)

set(TARGET_HEADER_FILES include/action.h include/client.h include/conf.h include/erreur.h include/exec.h include/game.h
        include/ipc.h include/message.h include/network.h include/parser.h include/private.h include/plugins.h
        include/public.h include/server.h)

add_executable(${TARGET} main.c init.c plugins.c exec.c ipc.c client.c message.c parser.c map.c map_parser.c bouvier.c
        game.c time.c pulse.c objects.c network.c network_connect.c ${TARGET_HEADER_FILES})

target_include_directories(${TARGET} PRIVATE include)

if (ENABLE_PASCAL)
    target_compile_definitions(${TARGET} PRIVATE -DENABLE_GPC)
    target_link_library(${TARGET} gpc)
endif()

target_compile_options(${TARGET} PRIVATE -rdynamic)
target_link_libraries(${TARGET} pulsarnet)

install(TARGETS ${TARGET} RUNTIME DESTINATION "bin")