cmake_minimum_required(VERSION 3.10)

project(PulsarNet)

set(TARGET pulsarnet)

set(TARGET_HEADER_FILES net_private.h)
set(TARGET_SOURCE_FILES check.c error.c misc.c open.c read.c write.c)

add_library(${TARGET} ${TARGET_SOURCE_FILES} ${TARGET_HEADER_FILES})

target_compile_options(${TARGET} PRIVATE -rdynamic)

install(TARGETS ${TARGET} RUNTIME DESTINATION "lib")