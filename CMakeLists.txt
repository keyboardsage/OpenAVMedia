cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMedia VERSION 0.1.0 LANGUAGES CXX)

include(cmake/CPM.cmake)

add_executable(OpenAVMedia main.cpp)

CPMAddPackage(
  NAME ogg
  VERSION 1.3.5
  GITHUB_REPOSITORY xiph/ogg
#  DOWNLOAD_ONLY True
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
    DEPENDS ogg
    COMMENT "Copying ogg files and directories..."
)
add_dependencies(OpenAVMedia copy_ogg_files)

#target_link_libraries(OpenAVMedia PRIVATE ogg)

add_custom_target(clean-libs
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/libs
    COMMENT "Cleaning build artifacts..."
)