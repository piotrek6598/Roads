cmake_minimum_required(VERSION 3.0)
project(Roads)

set(CMAKE_C_STANDARD 11)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)

set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra")

set(ROADS_ENGINE_SOURCE_FILES
        src/avl_map.c
        src/avl_map.h
        src/list.c
        src/list.h
        src/map.h
        src/map.c
        src/city.c
        src/city.h
        src/road.c
        src/road.h
        src/route.c
        src/route.h
        src/roads_types.h
        src/utils.c
        src/utils.h
        src/heap.c
        src/heap.h
        src/path.c
        src/path.h)
set(ROADS_INTERFACE_SOURCE_FILES
        src/map_main.c src/text_interface.c src/text_interface.h)

add_executable(Map ${ROADS_INTERFACE_SOURCE_FILES} ${ROADS_ENGINE_SOURCE_FILES})
#add_executable(MapTest src/map_tests.c ${ROADS_ENGINE_SOURCE_FILES})

find_package(Doxygen)
if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            )
endif (DOXYGEN_FOUND)
