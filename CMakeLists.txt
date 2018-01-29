cmake_minimum_required(VERSION 2.8)

project(ThePulsars)

# External cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Options for the project
option(BUILD_SERVER "Build server" ON)
# Currently does not build, do no enable
option(BUILD_GCLIENT "Build graphical client" OFF)
option(BUILD_METACLIENT "Build meta client" OFF)

option(ENABLE_PASCAL "Enable pascal support" OFF)
option(ENABLE_EFENCE "Enable ElectricFence" OFF)

add_compile_options(-Wall -Werror)
include_directories(include)

# Get version from Git
include(GetGitRevisionDescription)
git_describe(VERSION --tags --dirty=-dirty)

#parse the version information into pieces.
string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_MICRO "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+-(.*)" "\\1" VERSION_SHA1 "${VERSION}")

add_compile_options(-DV_MAJOR=${VERSION_MAJOR})
add_compile_options(-DV_MINOR=${VERSION_MINOR})
add_compile_options(-DV_MICRO=${VERSION_MICRO})
add_compile_options(-DV_SHA=${VERSION_SHA1})


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