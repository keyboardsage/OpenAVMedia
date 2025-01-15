cmake_minimum_required(VERSION 3.16.0)
project(simplewebm_example)

# Set compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -std=gnu++98")

# For UNIX, use pthread library dynamically.
# Note: This is the best approach because it depends on libc and its not recommended to
# statically link against libc on modern Linux to avoid runtime issues
if(UNIX)
    set(PTHREAD_LIB "pthread")  # Default to dynamic linking
endif()

# Create a static library for WebMDemuxer, VPXDecoder, and OpusVorbisDecoder
add_library(simplewebm STATIC
    WebMDemuxer.cpp
    VPXDecoder.cpp
    OpusVorbisDecoder.cpp
)

# FYI
message(STATUS "libsimplewebm CMAKE_CURRENT_SOURCE_DIR directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "libsimplewebm CMAKE_CURRENT_BINARY_DIR directory: ${CMAKE_CURRENT_BINARY_DIR}")

# Include directories
target_include_directories(simplewebm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../../libs/include/webm
    ${CMAKE_CURRENT_BINARY_DIR}/../../libs/include
)

# Link against archive libraries
target_link_libraries(simplewebm PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/../../libs/libvpx.a
    ${CMAKE_CURRENT_BINARY_DIR}/../../libs/libwebm.a
    ${CMAKE_CURRENT_BINARY_DIR}/../../libs/libopus.a
    ${CMAKE_CURRENT_BINARY_DIR}/../../libs/libvorbis.a
    ${CMAKE_CURRENT_BINARY_DIR}/../../libs/libogg.a
    "${PTHREAD_LIB}"
)

# Add executable
add_executable(simplewebm_example example.cpp WebMDemuxer.cpp VPXDecoder.cpp OpusVorbisDecoder.cpp)

# Include directories
target_include_directories(simplewebm_example PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../../libs/include/webm
    ${CMAKE_CURRENT_BINARY_DIR}/../../libs/include
)

# Link the executable against the static library
target_link_libraries(simplewebm_example PUBLIC simplewebm)