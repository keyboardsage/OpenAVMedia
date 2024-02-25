#!/usr/bin/env bash

# Use the first argument as the CMAKE_BINARY_DIR
CMAKE_BINARY_DIR="$1"

# Define the path for the lock file
LOCK_FILE="${CMAKE_BINARY_DIR}/mkvparser_patch.lock"

# Don't run the script if not necessary
if [ -f "$LOCK_FILE" ]; then
    echo "Mkvparser script has already been run. Exiting."
    exit 0
else
    echo "Mkvparser script is running for the first time."
    # Create the lock file to prevent future executions
    touch "$LOCK_FILE"
fi

# Check if the directory exists; if not, print an error message and exit
if [ ! -d "${CMAKE_BINARY_DIR}/_deps/webm-src/mkvparser/" ]; then
    echo "Directory ${CMAKE_BINARY_DIR}/_deps/webm-src/mkvparser/ does not exist."
    exit 1
fi

# Perform the sed operation and then move the modified file to replace the original
sed '0,/protected:/s/protected:.*//g' "${CMAKE_BINARY_DIR}/_deps/webm-src/mkvparser/mkvparser.h" > "${CMAKE_BINARY_DIR}/_deps/webm-src/mkvparser/mkvparser_modified.h"
mv "${CMAKE_BINARY_DIR}/_deps/webm-src/mkvparser/mkvparser_modified.h" "${CMAKE_BINARY_DIR}/_deps/webm-src/mkvparser/mkvparser.h"