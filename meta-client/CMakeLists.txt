cmake_minimum_required(VERSION 3.10)

project(MetaClient)

find_package(CJSON REQUIRED)

set(TARGET metaclient)

set(TARGET_HEADER_FILES include/mclient.h include/mysql.h include/tournois.h)
set(TARGET_SOURCE_FILES bitmaps.c gtk_main.c init.c main.c maps.c matchs.c menubar.c mysql.c mysql_list.c run_match.c
        run_tournois.c tournois.c)

add_executable(${TARGET} ${TARGET_SOURCE_FILES} ${TARGET_HEADER_FILES})

target_compile_options(${TARGET} PRIVATE -rdynamic)
target_link_libraries(${TARGET} pulsarnet gtkutil)

install(TARGETS ${TARGET} RUNTIME DESTINATION "bin")