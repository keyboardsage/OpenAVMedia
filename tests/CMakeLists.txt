cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMediaTests VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Enforce strict ISO C++ compliance

# first
add_executable(test1 test1.cpp)
target_include_directories(test1 PRIVATE ${OPENAVMEDIA_LIBS_DIR}/include ${OPENAVMEDIA_LIBS_DIR}/include/opus ${OPENAVMEDIA_LIBS_DIR}/include/SDL2)

if(UNIX)
    set(PTHREAD_LIB "pthread")
endif()
target_link_libraries(test1 PRIVATE
    ${OPENAVMEDIA_LIBS_DIR}/libsimplewebm.a
    ${OPENAVMEDIA_LIBS_DIR}/libvorbis.a
    ${OPENAVMEDIA_LIBS_DIR}/libvorbisenc.a
    ${OPENAVMEDIA_LIBS_DIR}/libvorbisfile.a
    ${OPENAVMEDIA_LIBS_DIR}/libopus.a
    ${OPENAVMEDIA_LIBS_DIR}/libopusfile.a
    ${OPENAVMEDIA_LIBS_DIR}/libogg.a
    ${OPENAVMEDIA_LIBS_DIR}/libFLAC.a
    ${OPENAVMEDIA_LIBS_DIR}/libvpx.a
    ${OPENAVMEDIA_LIBS_DIR}/libwebm.a
    ${OPENAVMEDIA_LIBS_DIR}/libsoloud.a
    ${OPENAVMEDIA_LIBS_DIR}/libSDL2.a
    ${OPENAVMEDIA_LIBS_DIR}/libSDL2main.a
    ${OPENAVMEDIA_LIBS_DIR}/libSDL2_mixer.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)

# second
add_executable(test2 test1.cpp)
target_include_directories(test2 PRIVATE ${OPENAVMEDIA_LIBS_DIR}/include ${OPENAVMEDIA_LIBS_DIR}/include/opus ${OPENAVMEDIA_LIBS_DIR}/include/SDL2)

target_link_libraries(test2 PRIVATE
    ${OPENAVMEDIA_LIBS_DIR}/libopenavmedia.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)

# third
add_executable(test3 test3.cpp)
target_include_directories(test3 PRIVATE ${OPENAVMEDIA_LIBS_DIR}/include)

target_link_libraries(test3 PRIVATE
    ${OPENAVMEDIA_LIBS_DIR}/libsimplewebm.a
    ${OPENAVMEDIA_LIBS_DIR}/libvorbis.a
    ${OPENAVMEDIA_LIBS_DIR}/libvorbisenc.a
    ${OPENAVMEDIA_LIBS_DIR}/libvorbisfile.a
    ${OPENAVMEDIA_LIBS_DIR}/libopus.a
    ${OPENAVMEDIA_LIBS_DIR}/libogg.a
    ${OPENAVMEDIA_LIBS_DIR}/libvpx.a
    ${OPENAVMEDIA_LIBS_DIR}/libwebm.a
#    ${OPENAVMEDIA_LIBS_DIR}/libsoloud.a # soloud isn't necessary for this test
    ${OPENAVMEDIA_LIBS_DIR}/libSDL2.a
    ${OPENAVMEDIA_LIBS_DIR}/libSDL2main.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)

# fourth
add_executable(test4 test3.cpp)
target_include_directories(test4 PRIVATE ${OPENAVMEDIA_LIBS_DIR}/include)

target_link_libraries(test4 PRIVATE
    ${OPENAVMEDIA_LIBS_DIR}/libopenavmedia.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)

# fifth
add_executable(test5 test5.cpp)
target_include_directories(test5 PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../tests/ ${OPENAVMEDIA_LIBS_DIR}/include)

target_link_libraries(test5 PRIVATE
    ${OPENAVMEDIA_LIBS_DIR}/libsimplewebm.a
    ${OPENAVMEDIA_LIBS_DIR}/libvorbis.a
    ${OPENAVMEDIA_LIBS_DIR}/libvorbisenc.a
    ${OPENAVMEDIA_LIBS_DIR}/libvorbisfile.a
    ${OPENAVMEDIA_LIBS_DIR}/libopus.a
    ${OPENAVMEDIA_LIBS_DIR}/libogg.a
    ${OPENAVMEDIA_LIBS_DIR}/libvpx.a
    ${OPENAVMEDIA_LIBS_DIR}/libwebm.a
    ${OPENAVMEDIA_LIBS_DIR}/libsoloud.a
    ${OPENAVMEDIA_LIBS_DIR}/libSDL2.a
    ${OPENAVMEDIA_LIBS_DIR}/libSDL2main.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)

# sixth
add_executable(test6 test5.cpp)
target_include_directories(test6 PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../tests/ ${OPENAVMEDIA_LIBS_DIR}/include)

target_link_libraries(test6 PRIVATE
    ${OPENAVMEDIA_LIBS_DIR}/libopenavmedia.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)

# seven
add_executable(test7 test7.cpp)
target_include_directories(test7 PRIVATE ${OPENAVMEDIA_LIBS_DIR}/include ${OPENAVMEDIA_LIBS_DIR}/include/SDL2)

target_link_libraries(test7 PRIVATE
    ${OPENAVMEDIA_LIBS_DIR}/libSDL2_mixer.a
    ${OPENAVMEDIA_LIBS_DIR}/libvorbisfile.a
    ${OPENAVMEDIA_LIBS_DIR}/libvorbis.a
    ${OPENAVMEDIA_LIBS_DIR}/libvorbisenc.a
    ${OPENAVMEDIA_LIBS_DIR}/libopusfile.a
    ${OPENAVMEDIA_LIBS_DIR}/libopus.a
    ${OPENAVMEDIA_LIBS_DIR}/libFLAC.a
    ${OPENAVMEDIA_LIBS_DIR}/libogg.a
    ${OPENAVMEDIA_LIBS_DIR}/libSDL2.a
    ${OPENAVMEDIA_LIBS_DIR}/libSDL2main.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)

# eight
add_executable(test8 test7.cpp)
target_include_directories(test8 PRIVATE ${OPENAVMEDIA_LIBS_DIR}/include ${OPENAVMEDIA_LIBS_DIR}/include/SDL2)

target_link_libraries(test8 PRIVATE
    ${OPENAVMEDIA_LIBS_DIR}/libopenavmedia.a
    ${PTHREAD_LIB}
    ${CMAKE_DL_LIBS}
)