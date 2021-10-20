# Locate and configure the downloaded CJSON library
#
# Defines the following variables:
#
#   CJSON_FOUND
#   CJSON_INCLUDE_DIRS - The directories needed on the include paths
#   CJSON_LIBRARIES - The libraries to link to

# set CJSON_PREFIX to directory where the current file file is located
get_filename_component(CJSON_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
if (NOT DEFINED CJSON_INSTALL_DIR)
    set(CJSON_INSTALL_DIR ${CMAKE_BINARY_DIR}/downloads/install/cjson)
endif()
set(CJSON_NAME cjson-external)

set(CJSON_FOUND TRUE)
set(CJSON_INCLUDE_DIRS ${CJSON_INSTALL_DIR}/include ${CJSON_INSTALL_DIR})
set(CJSON_INCLUDE_DIR ${CJSON_INCLUDE_DIRS})

if(WIN32)
    if(MINGW)
        set(CJSON_LIBRARIES ${CJSON_INSTALL_DIR}/lib/libcjson.a)
    else()
        set(CJSON_LIBRARIES optimized ${CJSON_INSTALL_DIR}/lib/libcjson.lib)
    endif()
else()
    set(CJSON_LIBRARIES ${CJSON_INSTALL_DIR}/lib/libcjson.a)
endif()

set(CJSON_LIBRARY ${CJSON_LIBRARIES})

mark_as_advanced(CJSON_INCLUDE_DIRS CJSON_INCLUDE_DIR CJSON_LIBRARIES CJSON_LIBRARY)
