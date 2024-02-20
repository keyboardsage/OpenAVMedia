cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMedia VERSION 0.1.0 LANGUAGES CXX)

# Main Compilation
#[[
find_library(MY_LIBRARY
    NAMES name_of_the_library_without_prefix_or_suffix
    PATHS /path/to/search/for/library
    NO_DEFAULT_PATH # Do not search system default paths
)
]]

add_executable(OpenAVMedia main.cpp)

#target_include_directories(OpenAVMedia PRIVATE {CMAKE_BINARY_DIR}/_deps/vorbis-src/include)
#target_link_libraries(OpenAVMedia PRIVATE Ogg Vorbis)