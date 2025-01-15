# patch_webm.cmake
cmake_minimum_required(VERSION 3.16)

# Expects the variable "webm_SOURCE_DIR" that tells the script where libwebm is cloned
if(NOT DEFINED webm_SOURCE_DIR)
  message(FATAL_ERROR "webm_SOURCE_DIR is not defined!")
endif()

# Expects the line number to remove
if(NOT DEFINED LINE)
  message(FATAL_ERROR "LINE is not defined!")
endif()

# Function that removes a line from the passed variables
function(remove_line VAR_NAME LINE_NUMBER)
    # Get the current variable value from the parent scope
    set(_current_text "${${VAR_NAME}}" CACHE INTERNAL "current text" FORCE)

    # Normalize line endings and split into lines
    string(REPLACE "\r\n" "\n" _normalized "${_current_text}")
    string(REPLACE ";" "~LF~" _all_lines "${_normalized}") # custom delimiter of ~LF~ to represent semicolon
    string(REPLACE "\n" ";" _all_lines "${_all_lines}") # create a list of lines by adding semi-colons

    # Remove the specified line
    set(_new_lines "")
    set(_index 1)
    foreach(_line IN LISTS _all_lines)
        if(NOT _index EQUAL ${LINE_NUMBER})
            list(APPEND _new_lines "${_line}")
        endif()
        math(EXPR _index "${_index} + 1")
    endforeach()

    # Join remaining lines and update the variable
    string(REPLACE ";" "\n" _joined "${_new_lines}")
    string(REPLACE "~LF~" ";" _joined "${_joined}")

    # Return value
    set(${VAR_NAME} "${_joined}" PARENT_SCOPE)
endfunction()


# Set a lock file path
set(LOCK_FILE "${CMAKE_CURRENT_BINARY_DIR}/mkvparser_patch.lock")

# Check if the lock file exists
if(EXISTS "${LOCK_FILE}")
    message(STATUS "Mkvparser patch has already been run. Skipping.")
else()
    message(STATUS "Mkvparser patch is running for the first time.")
    
    # Perform the sed-like operation that removes the "protected" keyword from the mkvparser class
    file(READ "${webm_SOURCE_DIR}/mkvparser/mkvparser.h" MKVPARSER_CONTENTS)
    remove_line(MKVPARSER_CONTENTS LINE)
    file(WRITE "${webm_SOURCE_DIR}/mkvparser/mkvparser.h" "${MKVPARSER_CONTENTS}")

    # Create the lock file to prevent re-execution
    file(WRITE "${LOCK_FILE}" "")
endif()
