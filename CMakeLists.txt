cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(OpenAVMedia VERSION 0.1.0 LANGUAGES CXX)

include(cmake/CPM.cmake)

CPMAddPackage(
  NAME ogg
  VERSION 1.3.4
  GITHUB_REPOSITORY xiph/ogg
#  DOWNLOAD_ONLY True
)

add_executable(OpenAVMedia main.cpp)