cmake_minimum_required(VERSION 3.10)

project(PulsarClient)

find_package(CJSON REQUIRED)

set(TARGET gclient)

set(TARGET_HEADER_FILES include/client.h include/client_gtk.h)
set(TARGET_SOURCE_FILES gamearea.c gen_bigmap.c gen_bigmap2.c gen_minimap.c gtk.c init.c load_map.c main.c mainbox.c
        map.c menubar.c minimap.c network.c pixbuf.c players.c stats.c statusbar.c toolbar.c)

add_executable(${TARGET} ${TARGET_SOURCE_FILES} ${TARGET_HEADER_FILES})

target_compile_options(${TARGET} PRIVATE -rdynamic)
target_link_libraries(${TARGET} pulsarnet gtkutil)

install(TARGETS ${TARGET} RUNTIME DESTINATION "bin")