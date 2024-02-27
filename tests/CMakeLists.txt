cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMediaTests VERSION 0.1.0 LANGUAGES CXX)

# first
add_executable(test1 test1.cpp)
target_include_directories(test1 PRIVATE ${CMAKE_BINARY_DIR}/libs/include)

if(UNIX)
    set(PTHREAD_LIB "pthread")
endif()
target_link_libraries(test1 PRIVATE
    ${CMAKE_BINARY_DIR}/libs/libvorbis.a
    ${CMAKE_BINARY_DIR}/libs/libopus.a
    ${CMAKE_BINARY_DIR}/libs/libogg.a
    ${CMAKE_BINARY_DIR}/libs/libvpx.a
    ${CMAKE_BINARY_DIR}/libs/libwebm.a
    ${CMAKE_BINARY_DIR}/libs/libsoloud.a
    ${CMAKE_BINARY_DIR}/libs/libSDL2.a
    ${CMAKE_BINARY_DIR}/libs/libSDL2main.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)

# second
add_executable(test2 test1.cpp)
target_include_directories(test2 PRIVATE ${CMAKE_BINARY_DIR}/libs/include)

target_link_libraries(test2 PRIVATE
    ${CMAKE_BINARY_DIR}/libs/libopenavmedia.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)