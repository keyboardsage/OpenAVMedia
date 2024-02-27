cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMedia VERSION 0.1.0 LANGUAGES CXX)

add_executable(OpenAVMediaTests test1.cpp)
target_include_directories(OpenAVMediaTests PRIVATE ${CMAKE_BINARY_DIR}/libs/include)
target_link_libraries(OpenAVMediaTests PRIVATE ${CMAKE_BINARY_DIR}/libs/libvorbis.a ${CMAKE_BINARY_DIR}/libs/libopus.a ${CMAKE_BINARY_DIR}/libs/libogg.a)