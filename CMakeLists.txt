cmake_minimum_required(VERSION 2.8)

project(ThePulsars)

option(BUILD_SERVER "Build server" ON)
# Currently does not build, do no enable
option(BUILD_GCLIENT "Build graphical client" OFF)
option(BUILD_METACLIENT "Build meta client" OFF)

option(ENABLE_PASCAL "Enable pascal support" OFF)
option(ENABLE_EFENCE "Enable ElectricFence" OFF)

add_compile_options(-Wall -Werror)
include_directories(include)

#add_subdirectory(pulsarnet)

if (BUILD_SERVER)
    add_subdirectory(server)
endif()

if (BUILD_GCLIENT OR BUILD_METACLIENT)
    add_subdirectory(gtkutil)
endif()

if (BUILD_GCLIENT)
    add_subdirectory(gclient)
endif()

if (BUILD_METACLIENT)
    add_subdirectory(meta-client)
endif()