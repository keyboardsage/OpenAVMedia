# OpenAVMedia
A subset of open source audio and video media types paired with a convenient build system. 

# Motivation
Open source projects such as vorbis, vpx, etc. utilize several different build systems; such as Makefile, Cmake, Premake, Bash files, Genie, Ninja, etc. Developers run into issues such as:

1. Configuring and compiling each project individually. (time-consuming)
2. Library compilation relies on the underlying OS. (links against the wrong library version)

The purpose of this project is to create a CMake file that will compile all of the audio/video projects and produce a single static library. This saves a developer working on an audio/video application such as an audio player, video player, game engine, etc. from having to collect all of these libraries and configure them themselves.

# Why Static Library
Different versions of the same shared object in theory will support a program equally. However, depending on the nature of the changes between them, its possible that the difference in version can break the program.

It is good practice to test your program with different versions of the same dynamic library to ensure a program is functioning correctly, especially when upgrading to a new version. Alternatively, you can use versioning and dependency management tools to specify the exact version of the dynamic library that your program requires. Doing so helps prevent unexpected issues that can arise due to differences in version.

So put simply, its a design decision to prevent versioning issues. 

# Who Needs This

# Libraries Included
## Included
[ogg](https://github.com/xiph/ogg)
[vorbis](https://github.com/xiph/vorbis)
[opus](https://github.com/xiph/opus)
[webm](https://github.com/webmproject/libwebm)
[vpx](https://github.com/webmproject/libvpx)

## Maybe in Future
[daala](https://github.com/xiph/daala)
[flac](https://github.com/xiph/flac)
[xspf](https://sourceforge.net/projects/libspiff/)
[qoa](https://github.com/phoboslab/qoa)

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