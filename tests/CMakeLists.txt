cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMediaTests VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Enforce strict ISO C++ compliance

# first
add_executable(test1 test1.cpp)
target_include_directories(test1 PRIVATE ${CMAKE_BINARY_DIR}/libs/include ${CMAKE_BINARY_DIR}/libs/include/opus ${CMAKE_BINARY_DIR}/libs/include/SDL2)

if(UNIX)
    set(PTHREAD_LIB "pthread")
endif()
target_link_libraries(test1 PRIVATE
    ${CMAKE_BINARY_DIR}/libs/libsimplewebm.a
    ${CMAKE_BINARY_DIR}/libs/libvorbis.a
    ${CMAKE_BINARY_DIR}/libs/libvorbisenc.a
    ${CMAKE_BINARY_DIR}/libs/libvorbisfile.a
    ${CMAKE_BINARY_DIR}/libs/libopus.a
    ${CMAKE_BINARY_DIR}/libs/libopusfile.a
    ${CMAKE_BINARY_DIR}/libs/libogg.a
    ${CMAKE_BINARY_DIR}/libs/libFLAC.a
    ${CMAKE_BINARY_DIR}/libs/libvpx.a
    ${CMAKE_BINARY_DIR}/libs/libwebm.a
    ${CMAKE_BINARY_DIR}/libs/libsoloud.a
    ${CMAKE_BINARY_DIR}/libs/libSDL2.a
    ${CMAKE_BINARY_DIR}/libs/libSDL2main.a
    ${CMAKE_BINARY_DIR}/libs/libSDL2_mixer.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)

# second
add_executable(test2 test1.cpp)
target_include_directories(test2 PRIVATE ${CMAKE_BINARY_DIR}/libs/include ${CMAKE_BINARY_DIR}/libs/include/opus ${CMAKE_BINARY_DIR}/libs/include/SDL2)

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

# seven
add_executable(test7 test7.cpp)
target_include_directories(test7 PRIVATE ${CMAKE_BINARY_DIR}/libs/include ${CMAKE_BINARY_DIR}/libs/include/SDL2)

target_link_libraries(test7 PRIVATE
    ${CMAKE_BINARY_DIR}/libs/libSDL2_mixer.a
    ${CMAKE_BINARY_DIR}/libs/libvorbisfile.a
    ${CMAKE_BINARY_DIR}/libs/libvorbis.a
    ${CMAKE_BINARY_DIR}/libs/libvorbisenc.a
    ${CMAKE_BINARY_DIR}/libs/libopusfile.a
    ${CMAKE_BINARY_DIR}/libs/libopus.a
    ${CMAKE_BINARY_DIR}/libs/libFLAC.a
    ${CMAKE_BINARY_DIR}/libs/libogg.a
    ${CMAKE_BINARY_DIR}/libs/libSDL2.a
    ${CMAKE_BINARY_DIR}/libs/libSDL2main.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)

# eight
add_executable(test8 test7.cpp)
target_include_directories(test8 PRIVATE ${CMAKE_BINARY_DIR}/libs/include ${CMAKE_BINARY_DIR}/libs/include/SDL2)

target_link_libraries(test8 PRIVATE
    ${CMAKE_BINARY_DIR}/libs/libopenavmedia.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)