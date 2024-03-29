cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMediaTests VERSION 0.1.0 LANGUAGES CXX)

# first
add_executable(test1 test1.cpp)
target_include_directories(test1 PRIVATE ${CMAKE_BINARY_DIR}/libs/include)

if(UNIX)
    set(PTHREAD_LIB "pthread")
endif()
target_link_libraries(test1 PRIVATE
    ${CMAKE_BINARY_DIR}/libs/libsimplewebm.a
    ${CMAKE_BINARY_DIR}/libs/libvorbis.a
    ${CMAKE_BINARY_DIR}/libs/libvorbisenc.a
    ${CMAKE_BINARY_DIR}/libs/libvorbisfile.a
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

# third
add_executable(test3 test3.cpp)
target_include_directories(test3 PRIVATE ${CMAKE_BINARY_DIR}/libs/include)

target_link_libraries(test3 PRIVATE
    ${CMAKE_BINARY_DIR}/libs/libsimplewebm.a
    ${CMAKE_BINARY_DIR}/libs/libvorbis.a
    ${CMAKE_BINARY_DIR}/libs/libvorbisenc.a
    ${CMAKE_BINARY_DIR}/libs/libvorbisfile.a
    ${CMAKE_BINARY_DIR}/libs/libopus.a
    ${CMAKE_BINARY_DIR}/libs/libogg.a
    ${CMAKE_BINARY_DIR}/libs/libvpx.a
    ${CMAKE_BINARY_DIR}/libs/libwebm.a
#    ${CMAKE_BINARY_DIR}/libs/libsoloud.a # soloud isn't necessary for this test
    ${CMAKE_BINARY_DIR}/libs/libSDL2.a
    ${CMAKE_BINARY_DIR}/libs/libSDL2main.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)

# fourth
add_executable(test4 test3.cpp)
target_include_directories(test4 PRIVATE ${CMAKE_BINARY_DIR}/libs/include)

target_link_libraries(test4 PRIVATE
    ${CMAKE_BINARY_DIR}/libs/libopenavmedia.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)

# fifth
add_executable(test5 test5.cpp)
target_include_directories(test5 PRIVATE ${CMAKE_BINARY_DIR}/../tests/ ${CMAKE_BINARY_DIR}/libs/include)

target_link_libraries(test5 PRIVATE
    ${CMAKE_BINARY_DIR}/libs/libsimplewebm.a
    ${CMAKE_BINARY_DIR}/libs/libvorbis.a
    ${CMAKE_BINARY_DIR}/libs/libvorbisenc.a
    ${CMAKE_BINARY_DIR}/libs/libvorbisfile.a
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

# sixth
add_executable(test6 test5.cpp)
target_include_directories(test6 PRIVATE ${CMAKE_BINARY_DIR}/../tests/ ${CMAKE_BINARY_DIR}/libs/include)

target_link_libraries(test6 PRIVATE
    ${CMAKE_BINARY_DIR}/libs/libopenavmedia.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)