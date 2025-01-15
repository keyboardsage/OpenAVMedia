# patch_sdl_mixer.cmake
cmake_minimum_required(VERSION 3.16)

# Expects the variable "sdl_mixer_SOURCE_DIR" that tells the script where SDL_mixer is cloned
if(NOT DEFINED sdl_mixer_SOURCE_DIR)
  message(FATAL_ERROR "sdl_mixer_SOURCE_DIR is not defined!")
endif()

# Read in the top-level SDL_mixer CMakeLists.txt
file(READ "${sdl_mixer_SOURCE_DIR}/CMakeLists.txt" SDL_MIXER_CMAKELISTS_CONTENTS)

# First, comment out the "Using system opusfile" + find_package(OpusFile REQUIRED)
string(REPLACE 
    "message(STATUS \"Using system opusfile\")"
    "# message(STATUS \"Using system opusfile\")"
    SDL_MIXER_CMAKELISTS_CONTENTS
    "${SDL_MIXER_CMAKELISTS_CONTENTS}")

string(REPLACE 
    "find_package(OpusFile REQUIRED)"
    "# find_package(OpusFile DISABLED_BY_PATCH)"
    SDL_MIXER_CMAKELISTS_CONTENTS
    "${SDL_MIXER_CMAKELISTS_CONTENTS}")

# Second, comment out the lines that do "target_link_libraries(... OpusFile::opusfile)"
string(REPLACE
"target_link_libraries(SDL2_mixer PRIVATE OpusFile::opusfile)"
"# target_link_libraries(SDL2_mixer PRIVATE OpusFile::opusfile)"
SDL_MIXER_CMAKELISTS_CONTENTS
"${SDL_MIXER_CMAKELISTS_CONTENTS}")

# Third, comment out the "Using system libxmp" + find_package(libxmp REQUIRED)
string(REPLACE 
    "message(STATUS \"Using system libxmp\")"
    "# message(STATUS \"Using system libxmp\")"
    SDL_MIXER_CMAKELISTS_CONTENTS
    "${SDL_MIXER_CMAKELISTS_CONTENTS}")

string(REPLACE 
    "find_package(libxmp REQUIRED)"
    "# find_package(libxmp DISABLED_BY_PATCH)"
    SDL_MIXER_CMAKELISTS_CONTENTS
    "${SDL_MIXER_CMAKELISTS_CONTENTS}")

# Fourth, force ignoring "Using system vorbisfile" + find_package(Vorbis)
string(REPLACE
  "message(STATUS \"Using system vorbisfile\")"
  "# message(STATUS \"Using system vorbisfile\" DISABLED_BY_PATCH)"
  SDL_MIXER_CMAKELISTS_CONTENTS
  "${SDL_MIXER_CMAKELISTS_CONTENTS}")

string(REPLACE
  "find_package(Vorbis REQUIRED)"
  "# find_package(Vorbis DISABLED_BY_PATCH)"
  SDL_MIXER_CMAKELISTS_CONTENTS
  "${SDL_MIXER_CMAKELISTS_CONTENTS}")

# Fifth, force ignoring “Using system SDL2” + find_package(SDL2)
string(REPLACE
  "sdl_find_sdl2(\${sdl2_target_name} \${SDL_REQUIRED_VERSION})"
  "# sdl_find_sdl2(\${sdl2_target_name} \${SDL_REQUIRED_VERSION}) DISABLED_BY_PATCH"
  SDL_MIXER_CMAKELISTS_CONTENTS
  "${SDL_MIXER_CMAKELISTS_CONTENTS}")

# Sixth, force ignoring “Using system libflac + find_package(FLAC)
string(REPLACE
  "message(STATUS \"Using system libflac\")"
  "# message(STATUS \"Using system libflac\")"
  SDL_MIXER_CMAKELISTS_CONTENTS
  "${SDL_MIXER_CMAKELISTS_CONTENTS}"
)

string(REPLACE
  "find_package(FLAC REQUIRED)"
  "# find_package(FLAC DISABLED_BY_PATCH)"
  SDL_MIXER_CMAKELISTS_CONTENTS
  "${SDL_MIXER_CMAKELISTS_CONTENTS}"
)

# Add targets to the SDL_mixer's CMakeLists.txt that it would have found
string(REPLACE
    "project(SDL2_mixer"
    "
# [BEGIN PATCH] ------------------------------------------------
# include(CMakeDependentOption)

# --------------------------------------------------------------------
# Provide a custom define for the vorbisfile static target.
# --------------------------------------------------------------------
  if(NOT TARGET Vorbis::vorbisfile)
    add_library(Vorbis::vorbisfile STATIC IMPORTED GLOBAL)
    set_target_properties(Vorbis::vorbisfile PROPERTIES
    IMPORTED_LOCATION \"${CMAKE_CURRENT_BINARY_DIR}/libs/libvorbisfile.a\"
    )
  endif()

# --------------------------------------------------------------------
# Provide a custom define for the SDL2 static target.
# --------------------------------------------------------------------
  if(NOT TARGET SDL2::SDL2-static)
    add_library(SDL2::SDL2-static STATIC IMPORTED GLOBAL)
    set_target_properties(SDL2::SDL2-static PROPERTIES
    IMPORTED_LOCATION \"${CMAKE_CURRENT_BINARY_DIR}/libs/libSDL2.a\"
    INTERFACE_INCLUDE_DIRECTORIES \"/home/sage/Projects/OpenAVMedia/build/libs/include/SDL2\"
    )
  endif()

# --------------------------------------------------------------------
# Provide a custom define for the opusfile static target.
# --------------------------------------------------------------------
  if(NOT TARGET OpusFile::opusfile)
    add_library(OpusFile::opusfile STATIC IMPORTED GLOBAL)
    set_target_properties(OpusFile::opusfile PROPERTIES
    IMPORTED_LOCATION \"${CMAKE_CURRENT_BINARY_DIR}/libs/libopusfile.a\"

    #  INTERFACE_INCLUDE_DIRECTORIES \"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/libs/include/opusfile\" \"$<INSTALL_INTERFACE:include/opusfile>\"
    )
  endif()

# --------------------------------------------------------------------
# Provide a custom define for the FLAC static target.
# --------------------------------------------------------------------
  if(NOT TARGET FLAC::FLAC)
    add_library(FLAC::FLAC STATIC IMPORTED GLOBAL)
    set_target_properties(FLAC::FLAC PROPERTIES
      IMPORTED_LOCATION \"${CMAKE_CURRENT_BINARY_DIR}/libs/libFLAC.a\"
    )
  endif()
# [END PATCH] ------------------------------------------------

project (SDL2_mixer
"
    SDL_MIXER_CMAKELISTS_CONTENTS "${SDL_MIXER_CMAKELISTS_CONTENTS}"
)

# Write the patched result back
file(WRITE "${sdl_mixer_SOURCE_DIR}/CMakeLists.txt" "${SDL_MIXER_CMAKELISTS_CONTENTS}")

message(STATUS "Patched SDL_mixer/CMakeLists.txt to skip find_package and sdl_find_sdl2 for SDL2, FLAC, Vorbis, OpusFile and libxmp.")