cmake_minimum_required (VERSION 3.14)

# include build-common if this is host project.
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../external/build-common)
get_property(build_common_included GLOBAL PROPERTY BUILD_COMMON_INCLUDED)

if(NOT build_common_included)
    include(buildCommonPre)
endif()

project(stela C CXX)

if(NOT build_common_included)
    include(buildCommon)
    # parse version
    by_parse_ver("${CMAKE_CURRENT_SOURCE_DIR}/../CHANGELOGS.md")
    set(VERSION_NAME "") # don't use version name in default.
endif()

# dependencies check:
by_check_dependency(NAMES bison MIN_VERSION 3.8.0)
by_check_dependency(NAMES flex MIN_VERSION 2.6.0)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../module ${CMAKE_CURRENT_BINARY_DIR}/module)
