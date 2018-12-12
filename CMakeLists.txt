cmake_minimum_required(VERSION 3.12)

include(CheckLibraryExists)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(GNUInstallDirs)


project(EZProfile
        VERSION 0.0.1
        LANGUAGES C)

set(CMAKE_C_STANDARD 99)

# CMakeFind folder
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)
find_package(OTF2 REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${OTF2_INCLUDE_PATH}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${OTF2_INCLUDE_PATH}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${OTF2_LIBRARY_PATH} -lotf2")

add_executable(ezprofile
        src/main.c
        src/callbacks.c
        src/callbacks.h
        src/datastructures.h
        )
target_link_libraries(ezprofile otf2)
