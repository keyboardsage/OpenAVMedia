cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMedia VERSION 0.1.0 LANGUAGES CXX)

include(cmake/CPM.cmake)

add_executable(OpenAVMedia main.cpp)

CPMAddPackage(
  NAME ogg
  VERSION 1.3.4
  GITHUB_REPOSITORY xiph/ogg
#  DOWNLOAD_ONLY True
)

# Copy ogg files/directories to their final build location under build/libs
add_custom_target(make_libs_directory
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs
    DEPENDS ogg
    COMMENT "Creating 'libs' directory..."
)
add_custom_target(make_libs_include_directory
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs/include
    DEPENDS make_libs_directory
    COMMENT "Creating 'libs/include' directory..."
)

add_custom_target(copy_ogg_directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/ogg-build/include/"
        "${CMAKE_BINARY_DIR}/libs/include/"
    DEPENDS make_libs_include_directory
    COMMENT "Copying ogg's config_types.h file..."
)
add_custom_target(copy_ogg_ogg_h_library
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/ogg-src/include/ogg/ogg.h"
        "${CMAKE_BINARY_DIR}/libs/include/ogg/"
    DEPENDS copy_ogg_directory
    COMMENT "Copying ogg's ogg.h file..."
)
add_custom_target(copy_ogg_os_types_h_library
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/ogg-src/include/ogg/os_types.h"
        "${CMAKE_BINARY_DIR}/libs/include/ogg/"
    DEPENDS copy_ogg_ogg_h_library
    COMMENT "Copying ogg's os_types.h file..."
)
add_custom_target(copy_ogg_static_library
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/_deps/ogg-build/libogg.a"
        "${CMAKE_BINARY_DIR}/libs/"
    DEPENDS copy_ogg_os_types_h_library
    COMMENT "Copying ogg's static library file..."
)
add_dependencies(OpenAVMedia copy_ogg_static_library)

#target_link_libraries(OpenAVMedia PRIVATE ogg)