cmake_minimum_required(VERSION 3.10)

project(GtkUtils)

set(TARGET gtkutil)

set(TARGET_SOURCE_FILES callbacks.c clist.c create.c filesel.c)

add_library(${TARGET} ${TARGET_SOURCE_FILES})

target_compile_options(${TARGET} PRIVATE -rdynamic)

install(TARGETS ${TARGET} RUNTIME DESTINATION "lib")