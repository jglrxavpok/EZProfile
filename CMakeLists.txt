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
list (APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)
find_package (OTF2 REQUIRED)

add_executable(ezprofile main.c)