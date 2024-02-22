cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMedia VERSION 0.1.0 LANGUAGES CXX)

# A target dedicated to setting up directories
add_custom_target(setup_final_destination
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs/include
    COMMENT "Creating libs and include directories inside build..."
)

# A target dedicated to cleaning up
add_custom_target(clean-libs
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/libs
    COMMENT "Cleaning build artifacts..."
)

# CPM
include(cmake/CPM.cmake)

CPMAddPackage(
  NAME Ogg
  VERSION 1.3.5
  GITHUB_REPOSITORY xiph/ogg
#  DOWNLOAD_ONLY True
)

set(Ogg_DIR {CMAKE_BINARY_DIR}/_deps/ogg-build CACHE PATH "Path to the Ogg library's CMake configuration")
CPMAddPackage(
  NAME Vorbis
  VERSION 1.3.7
  GITHUB_REPOSITORY xiph/vorbis
#  DOWNLOAD_ONLY True
)

CPMAddPackage(
  NAME Opus
  VERSION 1.4
  GITHUB_REPOSITORY xiph/opus
#  DOWNLOAD_ONLY True
)

#[[
CPMAddPackage(
  NAME SDL
  VERSION 2.30.0
  GITHUB_REPOSITORY libsdl-org/SDL
#  DOWNLOAD_ONLY True
)
]]

CPMAddPackage(
  NAME WEBM
  GITHUB_REPOSITORY webmproject/libwebm
  GIT_TAG libwebm-1.0.0.31
#  DOWNLOAD_ONLY True
)

CPMAddPackage(
  NAME VPX
  GITHUB_REPOSITORY webmproject/libvpx
  GIT_TAG v1.14.0
  DOWNLOAD_ONLY True
)

include(ExternalProject)
ExternalProject_Add(VPX_build
    SOURCE_DIR ${VPX_SOURCE_DIR}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --disable-examples --disable-tools --disable-docs
    BUILD_COMMAND $(MAKE) -j
    INSTALL_COMMAND ""
    BUILD_IN_SOURCE 1
)

# Copy ogg files/directories to their final build location under build/libs/
add_custom_target(copy_ogg_files
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs/include
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/ogg-build/include/"
        "${CMAKE_BINARY_DIR}/libs/include/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/ogg-src/include/ogg/ogg.h"
        "${CMAKE_BINARY_DIR}/libs/include/ogg/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/ogg-build/libogg.a"
        "${CMAKE_BINARY_DIR}/libs/"
    DEPENDS setup_final_destination
    COMMENT "Copying ogg files and directories..."
)

# Copy vorbis files/directories to their final build location under build/libs/
add_custom_target(copy_vorbis_files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/vorbis-src/include/"
        "${CMAKE_BINARY_DIR}/libs/include/"
    COMMAND ${CMAKE_COMMAND} -E remove
        "${CMAKE_BINARY_DIR}/libs/include/Makefile.am"
    COMMAND ${CMAKE_COMMAND} -E remove
        "${CMAKE_BINARY_DIR}/libs/include/vorbis/Makefile.am"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/vorbis-build/lib/libvorbis.a"
        "${CMAKE_BINARY_DIR}/libs/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/vorbis-build/lib/libvorbisenc.a"
        "${CMAKE_BINARY_DIR}/libs/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/vorbis-build/lib/libvorbisfile.a"
        "${CMAKE_BINARY_DIR}/libs/"
    DEPENDS copy_ogg_files
    COMMENT "Copying vorbis files and directories..."
)

# Copy opus files/directories to their final build location under build/libs/
add_custom_target(copy_opus_files
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/libs/include/opus/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/opus-src/include/"
        "${CMAKE_BINARY_DIR}/libs/include/opus/"
    COMMAND ${CMAKE_COMMAND} -E remove
        "${CMAKE_BINARY_DIR}/libs/include/opus/meson.build"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/opus-build/libopus.a"
        "${CMAKE_BINARY_DIR}/libs/"
    COMMENT "Copying opus files and directories..."
)

# Copy webm files/directories to their final build location under build/libs/
add_custom_target(copy_webm_files
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/libs/include/webm"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/libs/include/webm/mkvmuxer/"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/libs/include/webm/mkvparser/"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/libs/include/webm/common/"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/webm-src/mkvmuxer/"
        "${CMAKE_BINARY_DIR}/libs/include/webm/mkvmuxer"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/webm-src/mkvparser/"
        "${CMAKE_BINARY_DIR}/libs/include/webm/mkvparser"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/webm-src/common/"
        "${CMAKE_BINARY_DIR}/libs/include/webm/common"

    COMMAND ${CMAKE_COMMAND} -E remove
        "${CMAKE_BINARY_DIR}/libs/include/webm/mkvmuxer/*.cc"
        "${CMAKE_BINARY_DIR}/libs/include/webm/mkvparser/*.cc"
        "${CMAKE_BINARY_DIR}/libs/include/webm/common/*.cc"
    COMMAND ${CMAKE_COMMAND} -E remove
        "${CMAKE_BINARY_DIR}/libs/include/webm/common/common.sh"
    
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/webm-build/libwebm.a"
        "${CMAKE_BINARY_DIR}/libs/"
    DEPENDS setup_final_destination
    COMMENT "Copying webm files and directories..."
)

# Copy vpx files/directories to their final build location under build/libs/
add_custom_target(copy_vpx_files
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs/include/vpx
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/vpx-src/vpx/"
        "${CMAKE_BINARY_DIR}/libs/include/vpx"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/vpx-src/libvpx.a"
        "${CMAKE_BINARY_DIR}/libs/"
    DEPENDS setup_final_destination
    COMMENT "Copying vpx files and directories..."
)

# Compile the libraries
add_subdirectory("./tests")
add_dependencies(OpenAVMedia copy_vorbis_files copy_opus_files copy_webm_files copy_vpx_files)