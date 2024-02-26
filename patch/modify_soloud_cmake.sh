#!/usr/bin/env bash

# Use the first argument as the CMAKE_BINARY_DIR
CMAKE_BINARY_DIR="$1"

# Define the path for the lock file
LOCK_FILE="${CMAKE_BINARY_DIR}/soloud_patch.lock"

# Don't run the script if not necessary
if [ -f "$LOCK_FILE" ]; then
    echo "SoLoud script has already been run. Exiting."
    exit 0
else
    echo "SoLoud script is running for the first time."
    # Create the lock file to prevent future executions
    touch "$LOCK_FILE"
fi

# Check if the directory exists; if not, print an error message and exit
if [ ! -d "${CMAKE_BINARY_DIR}/_deps/soloud-src/" ]; then
    echo "Directory ${CMAKE_BINARY_DIR}/_deps/soloud-src/ does not exist."
    exit 1
fi

# Perform the sed operation and then move the modified file to replace the original
sed -e "s|find_package (SDL2 REQUIRED)|find_package (SDL2 REQUIRED HINTS ${CMAKE_BINARY_DIR}/libs/)\n\tmessage(STATUS \"SoLoud SDL2 HINT is: ${CMAKE_BINARY_DIR}/libs/\")|g" "${CMAKE_BINARY_DIR}/_deps/soloud-src/contrib/src.cmake" > "${CMAKE_BINARY_DIR}/_deps/soloud-src/contrib/src_modified.cmake"
mv "${CMAKE_BINARY_DIR}/_deps/soloud-src/contrib/src_modified.cmake" "${CMAKE_BINARY_DIR}/_deps/soloud-src/contrib/src.cmake"
sed -e "s|include_directories (\${SDL2_INCLUDE_DIR})|include_directories (${CMAKE_BINARY_DIR}/libs/include/SDL2/)\n\tmessage(STATUS \"SoLoud SDL2 Include directory is: ${CMAKE_BINARY_DIR}/libs/include/SDL2/\")|g" "${CMAKE_BINARY_DIR}/_deps/soloud-src/contrib/src.cmake" > "${CMAKE_BINARY_DIR}/_deps/soloud-src/contrib/src_modified.cmake"
mv "${CMAKE_BINARY_DIR}/_deps/soloud-src/contrib/src_modified.cmake" "${CMAKE_BINARY_DIR}/_deps/soloud-src/contrib/src.cmake"
#sed -e '/#include "SDL_config.h"/s/^/\/\//' "${CMAKE_BINARY_DIR}/_deps/soloud-src/src/backend/sdl2_static/soloud_sdl2_static.cpp" > "${CMAKE_BINARY_DIR}/_deps/soloud-src/src/backend/sdl2_static/soloud_sdl2_static_modified.cpp"
#mv "${CMAKE_BINARY_DIR}/_deps/soloud-src/src/backend/sdl2_static/soloud_sdl2_static_modified.cpp" "${CMAKE_BINARY_DIR}/_deps/soloud-src/src/backend/sdl2_static/soloud_sdl2_static.cpp"