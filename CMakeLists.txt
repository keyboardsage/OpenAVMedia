cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMedia VERSION 0.1.0 LANGUAGES CXX)

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

# A target dedicated to cleaning up
add_custom_target(clean-libs
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/libs
    COMMENT "Cleaning build artifacts..."
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

# Copy ogg files/directories to their final build location under build/libs/
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

# Compile the libraries
add_subdirectory("./tests")
add_dependencies(OpenAVMedia copy_vorbis_files copy_opus_files)