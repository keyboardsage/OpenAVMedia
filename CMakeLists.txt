cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMedia VERSION 0.1.0 LANGUAGES CXX)

include(ExternalProject) # Will be using this functionality to customize external projects throughout this script

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
    GITHUB_REPOSITORY keyboardsage/ogg # Using a custom version of Ogg that has ogg_version_string()
    GIT_TAG version_info
    GIT_SHALLOW TRUE
)

set(Ogg_DIR ${CMAKE_BINARY_DIR}/_deps/ogg-build CACHE PATH "Path to the Ogg library's CMake configuration")
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

# Compiles SDL2
set(sdl2_SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/sdl2-src/ CACHE PATH "Path to the SDL2 library's CMake configuration")
CPMAddPackage(
  NAME SDL2
  GITHUB_REPOSITORY libsdl-org/SDL
  GIT_TAG release-2.30.0
#  OPTIONS
#    "SDL_STATIC ON" "SDL_SHARED OFF"
  DOWNLOAD_ONLY True
)
ExternalProject_Add(sdl2_build
    SOURCE_DIR ${sdl2_SOURCE_DIR}
    BINARY_DIR ${sdl2_SOURCE_DIR}/build
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -DSDL_STATIC=ON -DSDL_SHARED=OFF ${sdl2_SOURCE_DIR}
    BUILD_COMMAND ${CMAKE_COMMAND} --build .
    INSTALL_COMMAND ""
)
ExternalProject_Add_Step(sdl2_build copy_sdl2_files
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/libs/include/SDL2"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/sdl2-src/include/"
        "${CMAKE_BINARY_DIR}/libs/include/SDL2"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/sdl2-src/build/libSDL2.a"
        "${CMAKE_BINARY_DIR}/libs/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/sdl2-src/build/libSDL2main.a"
        "${CMAKE_BINARY_DIR}/libs/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/sdl2-src/build/libSDL2_test.a"
        "${CMAKE_BINARY_DIR}/libs/"
    DEPENDS setup_final_destination
    COMMENT "Copying SDL2 files and directories..."
    DEPENDEES build
    DEPENDERS install
    ALWAYS TRUE
)

# Compiles webm with the modifications
CPMAddPackage(
  NAME WEBM
  GITHUB_REPOSITORY webmproject/libwebm
  GIT_TAG libwebm-1.0.0.31
  DOWNLOAD_ONLY True
)
include(ExternalProject)
if(NOT webm_SOURCE_DIR)
  set(webm_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_deps/webm-src")
endif()
ExternalProject_Add(webm_build
    SOURCE_DIR ${webm_SOURCE_DIR}
    BINARY_DIR ${webm_SOURCE_DIR}/build
    CONFIGURE_COMMAND ${CMAKE_COMMAND} ${webm_SOURCE_DIR}
    BUILD_COMMAND ${CMAKE_COMMAND} --build .
    INSTALL_COMMAND ""
)
ExternalProject_Add_Step(webm_build copy_CMakeLists_patch
    COMMAND /bin/sh -c "./../patch/modify_mkvparser.sh '${CMAKE_BINARY_DIR}'"
    COMMENT "Modifying Mkvparser header in webm..."
    DEPENDEES download
    DEPENDERS configure
    ALWAYS TRUE
)
ExternalProject_Add_Step(webm_build copy_compiled_webm_files
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
        "${CMAKE_BINARY_DIR}/_deps/webm-src/build/libwebm.a"
        "${CMAKE_BINARY_DIR}/libs/"
    COMMENT "Copying webm files and directories..."
    DEPENDEES build
    DEPENDERS install
    ALWAYS TRUE
)

# Compiles VPX
CPMAddPackage(
  NAME VPX
  GITHUB_REPOSITORY webmproject/libvpx
  GIT_TAG v1.14.0
  DOWNLOAD_ONLY True
)
ExternalProject_Add(VPX_build
    SOURCE_DIR ${VPX_SOURCE_DIR}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --disable-examples --disable-tools --disable-docs
    BUILD_COMMAND $(MAKE) -j
    INSTALL_COMMAND ""
    BUILD_IN_SOURCE 1
    DEPENDS webm_build
)

# Compiles libsimplewebm with the modifications
CPMAddPackage(
  NAME libsimplewebm
  GITHUB_REPOSITORY zaps166/libsimplewebm
  GIT_TAG master
  GIT_SHALLOW TRUE
  DOWNLOAD_ONLY True
)
ExternalProject_Add(libsimplewebm_build
    SOURCE_DIR ${libsimplewebm_SOURCE_DIR}
    BINARY_DIR ${libsimplewebm_SOURCE_DIR}/build
    CONFIGURE_COMMAND ${CMAKE_COMMAND} ${libsimplewebm_SOURCE_DIR}
    BUILD_COMMAND ${CMAKE_COMMAND} --build .
    INSTALL_COMMAND ""
    DEPENDS webm_build
    DEPENDS VPX_build
)
ExternalProject_Add_Step(libsimplewebm_build copy_CMakeLists_patch
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/../patch/CMakeLists.txt"
        "${CMAKE_BINARY_DIR}/_deps/libsimplewebm-src/"
    COMMENT "Copying over custom CMakeLists.txt..."
    DEPENDEES download
    DEPENDERS configure
    ALWAYS TRUE
)
ExternalProject_Add_Step(libsimplewebm_build remove_outdated_libm_patch
    COMMAND ${CMAKE_COMMAND} -E remove_directory
        "${CMAKE_BINARY_DIR}/_deps/libsimplewebm-src/libwebm/"
    COMMENT "Removing the outdated libwebm: ${CMAKE_BINARY_DIR}/_deps/libsimplewebm-src/libwebm/..."
    DEPENDEES download
    DEPENDERS configure
    ALWAYS TRUE
)

# Compiles soloud
CPMAddPackage(
  NAME soloud
  GITHUB_REPOSITORY jarikomppa/soloud
  GIT_TAG RELEASE_20200207
  DOWNLOAD_ONLY True
)
ExternalProject_Add(soloud_build
    SOURCE_DIR ${soloud_SOURCE_DIR}/contrib
    BINARY_DIR ${soloud_SOURCE_DIR}/contrib/build
    CONFIGURE_COMMAND ${CMAKE_COMMAND} ${soloud_SOURCE_DIR}/contrib
    BUILD_COMMAND ${CMAKE_COMMAND} --build ${soloud_SOURCE_DIR}/contrib/build
    INSTALL_COMMAND ""
    DEPENDS sdl2_build
)
ExternalProject_Add_Step(soloud_build run_soloud_patch
    COMMAND /bin/sh -c "./../patch/modify_soloud_cmake.sh '${CMAKE_BINARY_DIR}'"
    COMMENT "Modifying SoLoud CMakeLists.txt to rely on the statically built SDL2 library..."
    DEPENDEES download
    DEPENDERS configure
    ALWAYS TRUE
)
ExternalProject_Add_Step(soloud_build copy_compiled_soloud_files
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/libs/include/soloud/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/soloud-src/include/"
        "${CMAKE_BINARY_DIR}/libs/include/soloud"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/soloud-src/contrib/build/libsoloud.a"
        "${CMAKE_BINARY_DIR}/libs/"
    COMMENT "Copying SoLoud files and directories..."
    DEPENDEES build
    DEPENDERS install
    ALWAYS TRUE
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
    #[[
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/vorbis-build/lib/libvorbisenc.a"
        "${CMAKE_BINARY_DIR}/libs/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/vorbis-build/lib/libvorbisfile.a"
        "${CMAKE_BINARY_DIR}/libs/"
    ]]
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

# Combines the multiple libraries and places them into a singular libopenavmedia.a
add_custom_target(combine_into_singular_static_lib
    COMMAND /bin/sh -c "./../patch/combine_static_libs.sh"
    COMMENT "Combining all the static libraries into libopenavmedia.a..."
)

# Compile the libraries
add_subdirectory("./tests")

add_dependencies(opus vorbis)                 # Build vorbis first, it depends upon Ogg, kicks off Ogg build
add_dependencies(VPX_build opus)              # Build Opus next, Ogg should already be built
add_dependencies(copy_vorbis_files VPX_build) # Build VPX now, which will kick off WebM build

add_dependencies(copy_opus_files copy_vorbis_files)
add_dependencies(copy_vpx_files copy_opus_files)

add_dependencies(libsimplewebm_build copy_vpx_files)  # After VPX, Vorbis, and Opus are ready/copied
add_dependencies(soloud_build libsimplewebm_build)    # The necessities to build libsimplewebm_build exist, build
add_dependencies(combine_into_singular_static_lib soloud_build) # Build SoLoud, which triggers building SDL2
add_dependencies(test1 combine_into_singular_static_lib)  # Create the singular lib by combining
add_dependencies(test2 test1)  # Lastly, finish making the tests