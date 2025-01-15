cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMedia VERSION 0.1.0 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(ExternalProject) # Will be using this functionality to customize external projects throughout this script
include(FetchContent)    # For fetching libraries in place of CPMAddPackage DOWNLOAD_ONLY

# A target dedicated to setting up directories
add_custom_target(setup_final_destination
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/libs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/libs/include
    COMMENT "Creating libs and include directories inside build..."
)

# A target dedicated to cleaning up
add_custom_target(clean-libs
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/libs
    COMMENT "Cleaning build artifacts..."
)

# ------------------------------------------------------------------------------
# CPM
# ------------------------------------------------------------------------------
include(cmake/CPM.cmake)

# ------------------------------------------------------------------------------
# Options
# ------------------------------------------------------------------------------
option(BUILD_TESTS "Build test1..test8 targets" ON)

# ------------------------------------------------------------------------------
# SDL2 - CPM Downloaded and Built
# ------------------------------------------------------------------------------
CPMAddPackage(
    NAME SDL2
    #GITHUB_REPOSITORY libsdl-org/SDL
    #GIT_TAG release-2.30.0
    URL https://github.com/libsdl-org/SDL/releases/download/release-2.30.0/SDL2-2.30.0.zip
    VERSION release-2.30.0
    OPTIONS
        "SDL_SHARED OFF"    # or ON if you want shared
        "SDL_STATIC ON"
        "SDL_TEST ON"       # you mentioned you need SDL_test
        "SDL2_DISABLE_SDL2MAIN=OFF"
)

add_custom_target(copy_sdl2_files ALL
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/libs/include/SDL2"

    # Copy all public headers from SDL2's source. If you prefer
    # the post-build headers from SDL2_BINARY_DIR, that's also possible.
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${SDL2_SOURCE_DIR}/include"
            #"${SDL2_BINARY_DIR}/include"
            "${CMAKE_CURRENT_BINARY_DIR}/libs/include/SDL2"

    # Copy the built static libs from the actual SDL targets:
    #
    #   SDL2::SDL2-static
    #   SDL2::SDL2main
    #   SDL2::SDL2test
    #
    # We use generator expressions like $<TARGET_FILE:SDL2::SDL2-static> to find the .a (or .lib).
    COMMAND "${CMAKE_COMMAND}" -E copy
            "$<TARGET_FILE:SDL2::SDL2-static>"
            "${CMAKE_CURRENT_BINARY_DIR}/libs/libSDL2.a"

    # SDL2main only exists if SDL2_DISABLE_SDL2MAIN=OFF
    COMMAND "${CMAKE_COMMAND}" -E copy
            "$<TARGET_FILE:SDL2::SDL2main>"
            "${CMAKE_CURRENT_BINARY_DIR}/libs/libSDL2main.a"

    # SDL2_test only if SDL_TEST=ON
    COMMAND "${CMAKE_COMMAND}" -E copy
            "$<TARGET_FILE:SDL2::SDL2test>"
            "${CMAKE_CURRENT_BINARY_DIR}/libs/libSDL2_test.a"

    COMMENT "Copying SDL2 files and directories..."
)

# Make sure we only run copy_sdl2_files **after** SDL2 targets have built:
add_dependencies(copy_sdl2_files SDL2::SDL2-static setup_final_destination)

if(TARGET SDL2::SDL2main)
  add_dependencies(copy_sdl2_files SDL2::SDL2main setup_final_destination)
endif()

if(TARGET SDL2::SDL2test)
  add_dependencies(copy_sdl2_files SDL2::SDL2test setup_final_destination)
endif()

# ------------------------------------------------------------------------------
# Normal CPM usage for Ogg, Vorbis, and Opus (which are not purely DOWNLOAD_ONLY)
# ------------------------------------------------------------------------------
CPMAddPackage(
    NAME Ogg
    GITHUB_REPOSITORY keyboardsage/ogg # Using a custom version of Ogg that has ogg_version_string()
    GIT_TAG version_info
    GIT_SHALLOW TRUE
)

set(Ogg_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/ogg-build CACHE PATH "Path to the Ogg library's CMake configuration")

CPMAddPackage(
  NAME Vorbis
  VERSION 1.3.7
  GITHUB_REPOSITORY xiph/vorbis
  GIT_SHALLOW TRUE
)

CPMAddPackage(
  NAME Opus
  VERSION 1.4
  GITHUB_REPOSITORY xiph/opus
  GIT_SHALLOW TRUE
)

# ------------------------------------------------------------------------------
# WEBM - CPM Downloaded and Built
# ------------------------------------------------------------------------------
set(webm_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/libwebm-src) # I know the naming convention, so will define this vars in advance
set(webm_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/libwebm-build)

CPMAddPackage(
    WEBM
    GIT_REPOSITORY https://github.com/webmproject/libwebm.git
    GIT_TAG libwebm-1.0.0.31
    GIT_SHALLOW TRUE

    PATCH_COMMAND 
    ${CMAKE_COMMAND} -E echo "Modifying Mkvparser header in libwebm..." &&
    ${CMAKE_COMMAND} -Dwebm_SOURCE_DIR=${webm_SOURCE_DIR} -DLINE=24 -P "${CMAKE_CURRENT_SOURCE_DIR}/patch/patch_webm.cmake"
)

add_custom_target(copy_compiled_webm_files ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/libs/include/webm"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/libs/include/webm/mkvmuxer/"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/libs/include/webm/mkvparser/"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/libs/include/webm/common/"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${webm_SOURCE_DIR}/mkvmuxer/"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/webm/mkvmuxer"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${webm_SOURCE_DIR}/mkvparser/"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/webm/mkvparser"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${webm_SOURCE_DIR}/common/"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/webm/common"

    COMMAND ${CMAKE_COMMAND} -E remove
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/webm/mkvmuxer/*.cc"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/webm/mkvparser/*.cc"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/webm/common/*.cc"
    COMMAND ${CMAKE_COMMAND} -E remove
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/webm/common/common.sh"

    COMMAND ${CMAKE_COMMAND} -E copy
        "${webm_BUILD_DIR}/libwebm.a"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/"
    COMMENT "Copying webm files and directories..."
)

# Make sure we only run copy_compiled_webm_files **after** WEBM target has been built:
add_dependencies(copy_compiled_webm_files webm setup_final_destination)

# ------------------------------------------------------------------------------
# VPX
# ------------------------------------------------------------------------------
FetchContent_Declare(
  VPX
  GIT_REPOSITORY https://github.com/webmproject/libvpx.git
  GIT_TAG v1.14.0
)
FetchContent_GetProperties(VPX)
if(NOT VPX_POPULATED)
  FetchContent_MakeAvailable(VPX)
  message(STATUS "FetchContent_MakeAvailable completed for VPX.")
endif()

if(DEFINED vpx_SOURCE_DIR)
    set(vpx_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/vpx-build CACHE PATH "Path to the vpx library's build directory")
    message(STATUS "VPX source directory: ${vpx_SOURCE_DIR}")
    message(STATUS "VPX build directory: ${vpx_BUILD_DIR}")
else()
    message(FATAL_ERROR "vpx_SOURCE_DIR is not defined after FetchContent_MakeAvailable!")
endif()

ExternalProject_Add(VPX_build
    SOURCE_DIR ${vpx_SOURCE_DIR}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --disable-examples --disable-tools --disable-docs
    BUILD_COMMAND $(MAKE) -j
    INSTALL_COMMAND ""
    BUILD_IN_SOURCE 1
    #DEPENDS webm_build
)

# ------------------------------------------------------------------------------
# libsimplewebm
# ------------------------------------------------------------------------------
FetchContent_Declare(
  libsimplewebm
  GIT_REPOSITORY https://github.com/zaps166/libsimplewebm.git
  GIT_TAG master
)
FetchContent_GetProperties(libsimplewebm)
if(NOT libsimplewebm_POPULATED)
  FetchContent_MakeAvailable(libsimplewebm)
  message(STATUS "FetchContent_MakeAvailable completed for libsimplewebm.")
endif()

if(DEFINED libsimplewebm_SOURCE_DIR)
    set(libsimplewebm_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/libsimplewebm-build CACHE PATH "Path to the libsimplewebm library's build directory")
    message(STATUS "libsimplewebm source directory: ${libsimplewebm_SOURCE_DIR}")
    message(STATUS "libsimplewebm build directory: ${libsimplewebm_BUILD_DIR}")
else()
    message(FATAL_ERROR "libsimplewebm_SOURCE_DIR is not defined after FetchContent_MakeAvailable!")
endif()

ExternalProject_Add(libsimplewebm_build
    SOURCE_DIR ${libsimplewebm_SOURCE_DIR}
    BINARY_DIR ${libsimplewebm_BINARY_DIR}
    CONFIGURE_COMMAND ${CMAKE_COMMAND} ${libsimplewebm_SOURCE_DIR}
    BUILD_COMMAND ${CMAKE_COMMAND} --build .
    INSTALL_COMMAND ""
    #DEPENDS webm_build
    DEPENDS VPX_build
)

ExternalProject_Add_Step(libsimplewebm_build copy_CMakeLists_patch
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/../patch/CMakeLists.txt"
        "${libsimplewebm_SOURCE_DIR}/"
    COMMENT "Copying over custom CMakeLists.txt..."
    DEPENDEES download
    DEPENDERS configure
    ALWAYS TRUE
)
ExternalProject_Add_Step(libsimplewebm_build remove_outdated_libwebm_patch
    COMMAND ${CMAKE_COMMAND} -E remove_directory
        "${libsimplewebm_SOURCE_DIR}/libwebm/"
    COMMENT "Removing the outdated libwebm: ${libsimplewebm_SOURCE_DIR}/libwebm/..."
    DEPENDEES download
    DEPENDERS configure
    ALWAYS TRUE
)
ExternalProject_Add_Step(libsimplewebm_build copy_simplewebm_files
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/libs/include/simplewebm
    COMMAND ${CMAKE_COMMAND} -E copy
        "${libsimplewebm_BINARY_DIR}/libsimplewebm.a"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${libsimplewebm_SOURCE_DIR}/OpusVorbisDecoder.hpp"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/simplewebm/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${libsimplewebm_SOURCE_DIR}/VPXDecoder.hpp"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/simplewebm/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${libsimplewebm_SOURCE_DIR}/WebMDemuxer.hpp"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/simplewebm/"
    COMMENT "Copying simplewebm files and directories..."
    DEPENDEES build
    DEPENDERS install
    ALWAYS TRUE
)

# ------------------------------------------------------------------------------
# soloud
# ------------------------------------------------------------------------------
FetchContent_Declare(
  soloud
  GIT_REPOSITORY https://github.com/jarikomppa/soloud.git
  GIT_TAG RELEASE_20200207
)
FetchContent_GetProperties(soloud)
if(NOT soloud_POPULATED)
  FetchContent_Populate(soloud)
  message(STATUS "FetchContent_MakeAvailable completed for soloud.")
endif()

if(DEFINED soloud_SOURCE_DIR)
  set(soloud_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/soloud-build CACHE PATH "Path to the soloud library's build directory")
  message(STATUS "soloud source directory: ${soloud_SOURCE_DIR}")
  message(STATUS "soloud build directory: ${soloud_BUILD_DIR}")
else()
  message(FATAL_ERROR "soloud_SOURCE_DIR is not defined after FetchContent_MakeAvailable!")
endif()

ExternalProject_Add(soloud_build
    SOURCE_DIR ${soloud_SOURCE_DIR}/contrib
    BINARY_DIR ${soloud_SOURCE_DIR}/contrib/build
    CONFIGURE_COMMAND ${CMAKE_COMMAND} ${soloud_SOURCE_DIR}/contrib
    BUILD_COMMAND ${CMAKE_COMMAND} --build ${soloud_SOURCE_DIR}/contrib/build
    INSTALL_COMMAND ""
    DEPENDS copy_sdl2_files
)

ExternalProject_Add_Step(soloud_build run_soloud_patch
    COMMAND /bin/sh -c "${CMAKE_CURRENT_SOURCE_DIR}/patch/modify_soloud_cmake.sh '${CMAKE_CURRENT_BINARY_DIR}'"
    COMMENT "Modifying SoLoud CMakeLists.txt to rely on the statically built SDL2 library..."
    DEPENDEES download
    DEPENDERS configure
    ALWAYS TRUE
)
ExternalProject_Add_Step(soloud_build copy_compiled_soloud_files
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/libs/include/soloud/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${soloud_SOURCE_DIR}/include/"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/soloud"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${soloud_SOURCE_DIR}/contrib/build/libsoloud.a"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/"
    COMMENT "Copying SoLoud files and directories..."
    DEPENDEES build
    DEPENDERS install
    ALWAYS TRUE
)

# ------------------------------------------------------------------------------
# Normal CPM usage for libs Flac, opusfile - (which are not purely DOWNLOAD_ONLY)
# ------------------------------------------------------------------------------
CPMAddPackage(
  NAME Flac
  GIT_TAG 1.4.3
  GITHUB_REPOSITORY xiph/flac
  OPTIONS
    "BUILD_CXXLIBS OFF"
    "BUILD_EXAMPLES OFF"
    "BUILD_TESTING OFF"
    "BUILD_DOCS OFF"
    "INSTALL_MANPAGES OFF"
)

CPMAddPackage(
  NAME opusfile
#  GIT_VERSION v0.12
  GIT_TAG 9d718345ce03b2fad5d7d28e0bcd1cc69ab2b166
  GITHUB_REPOSITORY xiph/opusfile
  OPTIONS
    "OP_DISABLE_HTTP TRUE"
    "OP_DISABLE_DOCS TRUE"
    "OP_DISABLE_EXAMPLES TRUE"
)

set(OpusFile_INCLUDE_PATH "${CMAKE_CURRENT_BINARY_DIR}/libs/include/opusfile")
set(OpusFile_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/libs/libopusfile.a")

# ------------------------------------------------------------------------------
# SDL_mixer
# ------------------------------------------------------------------------------
FetchContent_Declare(
  SDL_mixer
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
  GIT_TAG release-2.8.0
)
FetchContent_GetProperties(SDL_mixer)
if(NOT SDL_mixer_POPULATED)
  FetchContent_Populate(SDL_mixer)
  message(STATUS "FetchContent_Populate completed for SDL_mixer. Fetched, but not configured.")
endif()

if(DEFINED sdl_mixer_SOURCE_DIR)
  message(STATUS "SDL_mixer source directory: ${sdl_mixer_SOURCE_DIR}")
  message(STATUS "SDL_mixer build directory: ${sdl_mixer_BINARY_DIR}")
else()
  message(FATAL_ERROR "sdl_mixer_SOURCE_DIR is not defined after FetchContent_Populate!")
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}" -Dsdl_mixer_SOURCE_DIR=${sdl_mixer_SOURCE_DIR}
          -P "${CMAKE_CURRENT_SOURCE_DIR}/patch/patch_sdl_mixer.cmake"
  WORKING_DIRECTORY "${sdl_mixer_SOURCE_DIR}"
)

# Compile SDL_mixer with the local SDL2, Opus, Ogg, and Vorbis libraries
set(SDL2_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/libs/include/SDL2")
set(SDL2_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/libs/libSDL2.a")
set(SDL2MAIN_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/libs/libSDL2main.a")

set(OPUS_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/libs/include/opus")
set(OPUS_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/libs/libopus.a")

set(OPUSFILE_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/libs/include/opusfile")
set(OPUSFILE_LOCAL_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/libs/libopusfile.a")

set(OGG_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/libs/include/ogg")
set(OGG_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/libs/libogg.a")

set(VORBIS_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/libs/include/vorbis")
set(VORBIS_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/libs/libvorbis.a")
set(VORBISENC_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/libs/libvorbisenc.a")
set(VORBISFILE_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/libs/libvorbisfile.a")

set(FLAC_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/libs/include/FLAC")
set(FLAC_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/libs/libFLAC.a")

ExternalProject_Add(sdl_mixer_build
    SOURCE_DIR        "${sdl_mixer_SOURCE_DIR}"
    BINARY_DIR        "${sdl_mixer_BINARY_DIR}"

    CONFIGURE_COMMAND ${CMAKE_COMMAND}
      -S ${sdl_mixer_SOURCE_DIR}
      -B ${sdl_mixer_BINARY_DIR}
      -DCMAKE_BUILD_TYPE=Release
      -DSDL2_DISABLE_FIND_SDL2=ON
      -DSDL_SHARED=OFF
      -DSDL_STATIC=ON
      -DBUILD_SHARED_LIBS=OFF
      -DSDL2MIXER_BUILD_SHARED_LIBS=OFF
      -DSDL2MIXER_VENDORED=OFF
      -DSDL2MIXER_OGG=ON
      -DSDL2MIXER_VORBIS=VORBISFILE
      -DSDL2MIXER_VORBIS_VORBISFILE_SHARED=OFF
      -DSDL2MIXER_OPUS=ON
      -DSDL2MIXER_OPUS_SHARED=OFF
      -DSDL2MIXER_BUILD_LOADOPUS_DYNAMIC=OFF
      -DSDL2MIXER_MP3=ON
      -DSDL2MIXER_MP3_MINIMP3=ON
      -DSDL2MIXER_FLAC=ON
      -DSDL2MIXER_FLAC_LIBFLAC=ON
      -DSDL2MIXER_FLAC_LIBFLAC_SHARED=OFF
      -DSDL2MIXER_GME=OFF
      -DSDL2MIXER_MOD=OFF
      -DSDL2MIXER_MOD_XMP=OFF
      -DSDL2MIXER_BUILD_LIBXMP=OFF
      -DSDL2MIXER_BUILD_LOADXMP_DYNAMIC=OFF
      -DSDL2MIXER_MIDI=OFF
      -DSDL2MIXER_WAVPACK=OFF
      -DSDL2MIXER_CMD=OFF

      -DSDL2_INCLUDE_DIR=${SDL2_INCLUDE_DIR}
      -DSDL2_LIBRARY=${SDL2_LIBRARY}
      -DSDL2MAIN_LIBRARY=${SDL2MAIN_LIBRARY}

      -DOGG_INCLUDE_DIR="${OGG_INCLUDE_DIR}"
      -DOGG_LIBRARY="${CMAKE_CURRENT_BINARY_DIR}/libs/libogg.a"

      -DVORBIS_INCLUDE_DIR="${VORBIS_INCLUDE_DIR}"
      -DVORBIS_LIBRARY="${CMAKE_CURRENT_BINARY_DIR}/libs/libvorbis.a"
      -DVORBISENC_LIBRARY="${CMAKE_CURRENT_BINARY_DIR}/libs/libvorbisenc.a"
      -DVORBISFILE_LIBRARY="${CMAKE_CURRENT_BINARY_DIR}/libs/libvorbisfile.a"

      -DCMAKE_C_FLAGS="-I${CMAKE_CURRENT_BINARY_DIR}/libs/include/opus"

      -DOPUS_INCLUDE_DIR="${OPUS_INCLUDE_DIR}"
      -DOPUS_LIBRARY="${CMAKE_CURRENT_BINARY_DIR}/libs/libopus.a"

      -DOpusFile_INCLUDE_PATH=${OPUSFILE_INCLUDE_DIR}
      -DOpusFile_LIBRARY=${OPUSFILE_LOCAL_LIBRARY}

      -DFLAC_INCLUDE_DIR=${FLAC_INCLUDE_DIR}
      -DFLAC_LIBRARY=${FLAC_LIBRARY}

      -DCMAKE_PREFIX_PATH="${CMAKE_CURRENT_BINARY_DIR}/libs"
      -DCMAKE_LIBRARY_PATH="${CMAKE_CURRENT_BINARY_DIR}/libs"
      -DCMAKE_INCLUDE_PATH="${CMAKE_CURRENT_BINARY_DIR}/libs/include"

      -DSDL2MIXER_INSTALL=OFF
      -DCMAKE_INSTALL_PREFIX="${CMAKE_CURRENT_BINARY_DIR}/libs"

    BUILD_COMMAND    ${CMAKE_COMMAND} --build ${sdl_mixer_BINARY_DIR} --target SDL2_mixer
    INSTALL_COMMAND  ""

    DEPENDS copy_sdl2_files
)

add_dependencies(sdl_mixer_build setup_final_destination)

ExternalProject_Add_Step(sdl_mixer_build copy_sdl_mixer_files
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/libs/include/SDL_mixer/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${sdl_mixer_SOURCE_DIR}/include/"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/SDL_mixer/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${sdl_mixer_BINARY_DIR}/libSDL2_mixer.a"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/"
    COMMENT "Copying SDL_mixer files and directories..."
    DEPENDEES build
    DEPENDERS install
    ALWAYS TRUE
)

# ------------------------------------------------------------------------------
# The rest of the custom copy_* targets
# ------------------------------------------------------------------------------
add_custom_target(copy_ogg_files
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/libs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/libs/include
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/ogg-build/include/"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/ogg-src/include/ogg/ogg.h"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/ogg/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/ogg-build/libogg.a"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/"
    DEPENDS setup_final_destination
    COMMENT "Copying ogg files and directories..."
)

add_custom_target(copy_vorbis_files
    COMMAND make -C "${CMAKE_CURRENT_BINARY_DIR}/_deps/vorbis-build"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/vorbis-src/include/"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/"
    COMMAND ${CMAKE_COMMAND} -E remove
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/Makefile.am"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/vorbis/Makefile.am"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/vorbis-build/lib/libvorbis.a"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/vorbis-build/lib/libvorbisenc.a"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/vorbis-build/lib/libvorbisfile.a"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/"
    DEPENDS copy_ogg_files
    COMMENT "Building libvorbisenc.a and libvorbisfile.a then copying vorbis files and directories..."
)

add_custom_target(copy_opus_files
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/libs/include/opus/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/opus-src/include/"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/opus/"
    COMMAND ${CMAKE_COMMAND} -E remove
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/opus/meson.build"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/opus-build/libopus.a"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/"
    COMMENT "Copying opus files and directories..."
)

add_custom_target(copy_vpx_files
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/libs/include/vpx
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/vpx-src/vpx/"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/vpx"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/vpx-src/libvpx.a"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/"
    DEPENDS setup_final_destination
    COMMENT "Copying vpx files and directories..."
)

add_custom_target(copy_flac_files
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/libs/include/FLAC
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/flac-src/include/FLAC/"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/FLAC"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/flac-build/src/libFLAC/libFLAC.a"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/"
    DEPENDS setup_final_destination
    COMMENT "Copying flac files and directories..."
)

add_custom_target(copy_opusfile_files
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/libs/include/opusfile
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/opusfile-src/include/"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/opusfile"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/opusfile-build/libopusfile.a"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/"
    
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/libs/include/opus/opus
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/opusfile-src/include/"
        "${CMAKE_CURRENT_BINARY_DIR}/libs/include/opus/opus"

    DEPENDS setup_final_destination
    COMMENT "Copying opusfile files and directories..."
)

# ------------------------------------------------------------------------------
# Combines the multiple libraries and places them into a singular libopenavmedia.a
# ------------------------------------------------------------------------------
add_custom_target(combine_into_singular_static_lib
    COMMAND /bin/sh -c "${CMAKE_CURRENT_SOURCE_DIR}/patch/combine_static_libs.sh"
    COMMENT "Combining all the static libraries into libopenavmedia.a..."
)

# ------------------------------------------------------------------------------
# Compile the libraries in a specified build order
# ------------------------------------------------------------------------------
set(OPENAVMEDIA_LIBS_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs) # The "tests/CMakeLists.txt" will use this variable
add_subdirectory("./tests")

add_dependencies(opus vorbis)                 # Build vorbis first, it depends upon Ogg
add_dependencies(VPX_build opus)              # Then build VPX
add_dependencies(copy_vorbis_files VPX_build) # Build VPX now, which will kick off WebM build

add_dependencies(copy_opus_files copy_vorbis_files)
add_dependencies(copy_vpx_files copy_opus_files)

add_dependencies(libsimplewebm_build copy_vpx_files)
add_dependencies(soloud_build libsimplewebm_build)
add_dependencies(opusfile soloud_build)
add_dependencies(FLAC opusfile)
add_dependencies(copy_opusfile_files FLAC)
add_dependencies(copy_flac_files copy_opusfile_files)
add_dependencies(sdl_mixer_build copy_flac_files)
add_dependencies(combine_into_singular_static_lib sdl_mixer_build)

if(BUILD_TESTS)
    add_dependencies(test1 combine_into_singular_static_lib)
    add_dependencies(test2 test1)
    add_dependencies(test3 test2)
    add_dependencies(test4 test3)
    add_dependencies(test5 test4)
    add_dependencies(test6 test5)
    add_dependencies(test7 test6)
    add_dependencies(test8 test7)
endif()

# ------------------------------------------------------------------------------
# Interface for OpenAVMedia below.
# ------------------------------------------------------------------------------
add_custom_target(openavmedia_full_build ALL
    COMMENT "Aggregates all build steps in OpenAVMedia"
)
if(BUILD_TESTS) # set interface dependencies
    add_dependencies(openavmedia_full_build combine_into_singular_static_lib test8)
    message(STATUS "Including test1..test8 as BUILD_TESTS=ON")
else()
    add_dependencies(openavmedia_full_build combine_into_singular_static_lib)
    message(STATUS "Skipping test1..test8 as BUILD_TESTS=OFF")
endif()

# Start by creating an interface library.
add_library(OpenAVMedia INTERFACE)

# Force the "openavmedia_full_build" aggregator to build first whenever something links to OpenAVMedia
add_dependencies(OpenAVMedia openavmedia_full_build)

# Specify include directories (if any)
target_include_directories(OpenAVMedia INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/libs/include
    ${CMAKE_CURRENT_BINARY_DIR}/libs/include/SDL2
)

# Export the target with a namespace
add_library(OpenAVMedia::OpenAVMedia ALIAS OpenAVMedia)