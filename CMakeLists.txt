cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMedia VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)

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
  set(webm_SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/webm-src")
endif()
ExternalProject_Add(webm_build
    SOURCE_DIR ${webm_SOURCE_DIR}
    BINARY_DIR ${webm_SOURCE_DIR}/build
    CONFIGURE_COMMAND ${CMAKE_COMMAND} ${webm_SOURCE_DIR}
    BUILD_COMMAND ${CMAKE_COMMAND} --build .
    INSTALL_COMMAND ""
)
ExternalProject_Add_Step(webm_build copy_CMakeLists_patch
    COMMAND /bin/sh -c "${CMAKE_SOURCE_DIR}/patch/modify_mkvparser.sh '${CMAKE_BINARY_DIR}'"
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
ExternalProject_Add_Step(libsimplewebm_build remove_outdated_libwebm_patch
    COMMAND ${CMAKE_COMMAND} -E remove_directory
        "${CMAKE_BINARY_DIR}/_deps/libsimplewebm-src/libwebm/"
    COMMENT "Removing the outdated libwebm: ${CMAKE_BINARY_DIR}/_deps/libsimplewebm-src/libwebm/..."
    DEPENDEES download
    DEPENDERS configure
    ALWAYS TRUE
)
ExternalProject_Add_Step(libsimplewebm_build copy_simplewebm_files
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs/include/simplewebm
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/libsimplewebm-src/build/libsimplewebm.a"
        "${CMAKE_BINARY_DIR}/libs/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/libsimplewebm-src/OpusVorbisDecoder.hpp"
        "${CMAKE_BINARY_DIR}/libs/include/simplewebm/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/libsimplewebm-src/VPXDecoder.hpp"
        "${CMAKE_BINARY_DIR}/libs/include/simplewebm/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/libsimplewebm-src/WebMDemuxer.hpp"
        "${CMAKE_BINARY_DIR}/libs/include/simplewebm/"
    COMMENT "Copying simplewebm files and directories..."
    DEPENDEES build
    DEPENDERS install
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
    COMMAND /bin/sh -c "${CMAKE_SOURCE_DIR}/patch/modify_soloud_cmake.sh '${CMAKE_BINARY_DIR}'"
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

CPMAddPackage(
  NAME Flac
  GIT_TAG 1.4.3
  GITHUB_REPOSITORY xiph/flac
#  DOWNLOAD_ONLY True
  OPTIONS
    "BUILD_CXXLIBS OFF"
    "BUILD_EXAMPLES OFF"
    "BUILD_TESTING OFF"
    "BUILD_DOCS OFF"
    "INSTALL_MANPAGES OFF"
)

# Install opusfile for SDL_mixer later
CPMAddPackage(
  NAME opusfile
#  GIT_VERSION v0.12
  GIT_TAG 9d718345ce03b2fad5d7d28e0bcd1cc69ab2b166
  GITHUB_REPOSITORY xiph/opusfile
#  DOWNLOAD_ONLY True
  OPTIONS
    "OP_DISABLE_HTTP TRUE"
    "OP_DISABLE_DOCS TRUE"
    "OP_DISABLE_EXAMPLES TRUE"
)

# Compiles SDL_mixer
CPMAddPackage(
  NAME SDL_mixer
  GITHUB_REPOSITORY libsdl-org/SDL_mixer
  GIT_TAG release-2.8.0
  DOWNLOAD_ONLY TRUE
)

# Compile SDL_mixer with the local SDL2, Opus, Ogg, and Vorbis libraries
set(SDL2_INCLUDE_DIR "${CMAKE_BINARY_DIR}/libs/include/SDL2")
set(SDL2_LIBRARY "${CMAKE_BINARY_DIR}/libs/libSDL2.a")
set(SDL2MAIN_LIBRARY "${CMAKE_BINARY_DIR}/libs/libSDL2main.a")

set(OPUS_INCLUDE_DIR "${CMAKE_BINARY_DIR}/libs/include/opus")
set(OPUS_LIBRARY "${CMAKE_BINARY_DIR}/libs/libopus.a")

set(OPUSFILE_INCLUDE_DIR "${CMAKE_BINARY_DIR}/libs/include/opusfile")
set(OPUSFILE_LIBRARY "${CMAKE_BINARY_DIR}/libs/libopusfile.a")

set(OGG_INCLUDE_DIR "${CMAKE_BINARY_DIR}/libs/include/ogg")
set(OGG_LIBRARY "${CMAKE_BINARY_DIR}/libs/libogg.a")

set(VORBIS_INCLUDE_DIR "${CMAKE_BINARY_DIR}/libs/include/vorbis")
set(VORBIS_LIBRARY "${CMAKE_BINARY_DIR}/libs/libvorbis.a")
set(VORBISENC_LIBRARY "${CMAKE_BINARY_DIR}/libs/libvorbisenc.a")
set(VORBISFILE_LIBRARY "${CMAKE_BINARY_DIR}/libs/libvorbisfile.a")

set(FLAC_INCLUDE_DIR "${CMAKE_BINARY_DIR}/libs/include/FLAC")
set(FLAC_LIBRARY "${CMAKE_BINARY_DIR}/libs/libFLAC.a")

set(sdl_mixer_SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/sdl_mixer-src)
set(sdl_mixer_BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/sdl_mixer-build)

ExternalProject_Add(sdl_mixer_build
    SOURCE_DIR        "${sdl_mixer_SOURCE_DIR}"
    BINARY_DIR        "${sdl_mixer_BINARY_DIR}"

    # -------------------------------------------------------------------
    # Pass all the necessary defines so that SDL_mixer uses local libs.
    # These are the *most critical* flags:
    # -------------------------------------------------------------------
    CONFIGURE_COMMAND ${CMAKE_COMMAND}
      -S ${sdl_mixer_SOURCE_DIR}
      -B ${sdl_mixer_BINARY_DIR}
      -DCMAKE_BUILD_TYPE=Release  # Build in release mode
      -DSDL2_DISABLE_FIND_SDL2=ON # Telling SDL_mixer to disable attempts to find system SDL2

      # Telling SDL_mixer to build purely static, i.e. no .so, .dll, etc.
      -DSDL_SHARED=OFF
      -DSDL_STATIC=ON
      -DBUILD_SHARED_LIBS=OFF
      -DSDL2MIXER_BUILD_SHARED_LIBS=OFF

      -DSDL2MIXER_VENDORED=OFF         # Telling SDL_mixer not to fetch external libs (we have them locally)

      # Control which decoders to enable
      -DSDL2MIXER_OGG=ON
      -DSDL2MIXER_VORBIS=VORBISFILE    # or STB, TREMOR, etc.
      -DSDL2MIXER_VORBIS_VORBISFILE_SHARED=OFF
      -DSDL2MIXER_OPUS=ON
      -DSDL2MIXER_OPUS_SHARED=OFF
      -DSDL2MIXER_MP3=ON
      -DSDL2MIXER_MP3_MINIMP3=ON       # or MPG123 if you prefer
      -DSDL2MIXER_FLAC=ON
      -DSDL2MIXER_FLAC_LIBFLAC=ON
      -DSDL2MIXER_FLAC_LIBFLAC_SHARED=OFF

      # and which types are not going to be supported
      -DSDL2MIXER_GME=OFF        # no game-music-emu
      -DSDL2MIXER_MOD=OFF        # no MOD music
      -DSDL2MIXER_MIDI=OFF       # no MIDI
      -DSDL2MIXER_WAVPACK=OFF    # no WavPack
      -DSDL2MIXER_CMD=OFF        # no external command music player

      # Point SDL_mixer to the local SDL2
      -DSDL2_INCLUDE_DIR=${SDL2_INCLUDE_DIR}       # Path to ... /include/SDL2
      -DSDL2_LIBRARY=${SDL2_LIBRARY}               # Path to ... /libs/libSDL2.a
      -DSDL2MAIN_LIBRARY=${SDL2MAIN_LIBRARY}       # Path to ... /libs/libSDL2main.a

      # Force SDL_mixer to pick the local Ogg
      -DOGG_INCLUDE_DIR="${OGG_INCLUDE_DIR}"         # Path to ... /libs/include/ogg
      -DOGG_LIBRARY="${CMAKE_BINARY_DIR}/libs/libogg.a"

      # Force SDL_mixer to pick the local Vorbis
      -DVORBIS_INCLUDE_DIR="${VORBIS_INCLUDE_DIR}"   # Path to ... /libs/include/vorbis
      -DVORBIS_LIBRARY="${CMAKE_BINARY_DIR}/libs/libvorbis.a"
      -DVORBISENC_LIBRARY="${CMAKE_BINARY_DIR}/libs/libvorbisenc.a"
      -DVORBISFILE_LIBRARY="${CMAKE_BINARY_DIR}/libs/libvorbisfile.a"

      # Correctly point to the directory containing opus/opusfile.h
      -DCMAKE_C_FLAGS="-I${CMAKE_BINARY_DIR}/libs/include/opus"

      # Force SDL_mixer to pick the local Opus
      -DOPUS_INCLUDE_DIR="${OPUS_INCLUDE_DIR}"       # Path to ... /libs/include/opus
      -DOPUS_LIBRARY="${CMAKE_BINARY_DIR}/libs/libopus.a"

      # Force SDL_mixer to pick the local OpusFile
      -DOpusFile_INCLUDE_PATH=${OPUSFILE_INCLUDE_DIR}
      -DOpusFile_LIBRARY=${CMAKE_BINARY_DIR}/libs/libopusfile.a

      # Force SDL_mixer to pick the local FLAC
      -DFLAC_INCLUDE_DIR=${FLAC_INCLUDE_DIR}
      -DFLAC_LIBRARY=${FLAC_LIBRARY}

      # Make sure we search local paths first
      -DCMAKE_PREFIX_PATH="${CMAKE_BINARY_DIR}/libs" 
      -DCMAKE_LIBRARY_PATH="${CMAKE_BINARY_DIR}/libs"
      -DCMAKE_INCLUDE_PATH="${CMAKE_BINARY_DIR}/libs/include"

      # Provide an install prefix (even if SDL2MIXER_INSTALL=OFF)
      -DSDL2MIXER_INSTALL=OFF           # We are not going to "make install" SDL_mixer, so turned it off
      -DCMAKE_INSTALL_PREFIX="${CMAKE_BINARY_DIR}/libs"
    # -------------------------------------------------------------------

    BUILD_COMMAND    ${CMAKE_COMMAND} --build ${sdl_mixer_BINARY_DIR} --target SDL2_mixer
    INSTALL_COMMAND  ""

    DEPENDS sdl2_build
)

ExternalProject_Add_Step(sdl_mixer_build copy_sdl_mixer_files
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/libs/include/SDL_mixer/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${sdl_mixer_SOURCE_DIR}/include/"
        "${CMAKE_BINARY_DIR}/libs/include/SDL_mixer/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${sdl_mixer_BINARY_DIR}/libSDL2_mixer.a"
        "${CMAKE_BINARY_DIR}/libs/"
    COMMENT "Copying SDL_mixer files and directories..."
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
    # Make sure that vorbisenc and vorbisfile are made also
    COMMAND make -C "${CMAKE_BINARY_DIR}/_deps/vorbis-build"

    # Now copy the files
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
    COMMENT "Building libvorbisenc.a and libvorbisfile.a then copying vorbis files and directories..."
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

# Copy flac files/directories to their final build location under build/libs/
add_custom_target(copy_flac_files
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs/include/FLAC
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/flac-src/include/FLAC/"
        "${CMAKE_BINARY_DIR}/libs/include/FLAC"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/flac-build/src/libFLAC/libFLAC.a"
        "${CMAKE_BINARY_DIR}/libs/"
    DEPENDS setup_final_destination
    COMMENT "Copying flac files and directories..."
)

# Copy opusfile files/directories to their final build location under build/libs/
add_custom_target(copy_opusfile_files
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs/include/opusfile
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/opusfile-src/include/"
        "${CMAKE_BINARY_DIR}/libs/include/opusfile"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/opusfile-build/libopusfile.a"
        "${CMAKE_BINARY_DIR}/libs/"
    
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs/include/opus/opus
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/opusfile-src/include/"
        "${CMAKE_BINARY_DIR}/libs/include/opus/opus"

    DEPENDS setup_final_destination
    COMMENT "Copying opusfile files and directories..."
)

# Combines the multiple libraries and places them into a singular libopenavmedia.a
add_custom_target(combine_into_singular_static_lib
    COMMAND /bin/sh -c "${CMAKE_SOURCE_DIR}/patch/combine_static_libs.sh"
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
add_dependencies(opusfile soloud_build)               # Build SoLoud, which triggers building SDL2
add_dependencies(FLAC opusfile)                       # Build opusfile
add_dependencies(copy_opusfile_files FLAC)                # Build flac
add_dependencies(copy_flac_files copy_opusfile_files)     # Ready/Copy the OpusFile library
add_dependencies(sdl_mixer_build copy_flac_files)         # Ready/Copy the Flac library
add_dependencies(combine_into_singular_static_lib sdl_mixer_build)  # Build sdl_mixer
add_dependencies(test1 combine_into_singular_static_lib)  # Create the singular lib by combining
add_dependencies(test2 test1)  # Lastly, finish making the tests
add_dependencies(test3 test2)
add_dependencies(test4 test3)
add_dependencies(test5 test4)
add_dependencies(test6 test5)
add_dependencies(test7 test6)
add_dependencies(test8 test7)

########################################
# Interface for OpenAVMedia below.
########################################
# Start by creating an interface library.
add_library(OpenAVMedia INTERFACE)

# Specify include directories (if any)
target_include_directories(OpenAVMedia INTERFACE
    ${CMAKE_BINARY_DIR}/libs/include
    ${CMAKE_BINARY_DIR}/libs/include/SDL2
)

# Export the target with a namespace
add_library(OpenAVMedia::OpenAVMedia ALIAS OpenAVMedia)