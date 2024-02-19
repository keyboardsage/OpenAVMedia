# OpenAVMedia
A subset of open source audio and video media types paired with a convenient build system. 

# Motivation
Open source projects such as vorbis, vpx, etc. utilize several different build systems; such as Makefile, Cmake, Premake, Bash files, Genie, Ninja, etc. The current issues a developer runs into include:

1. Configuring and compiling each project individually is time-consuming.
2. You don't want your project's library version to rely solely on the OS you happen to use for compiling that day.
3. Using a single implementation/compilation for a library allows for a more predictable runtime and root cause analysis.

The purpose of this project is to create a CMake script that will compile these audio/video projects. This way a programmer can simply include this singular project and get the necessary static libraries and headers.

# Building
Go into the build directory. Create the cmake files using the CMakeLists.txt and make.
```
cd build
cmake ..
make
```
The static file for the libraries built will be under `build/libs` and their include files will be under `build/libs/include`.

# Cleaning
Go into the build directory and run these commands.
```
make clean
make clean-libs
```