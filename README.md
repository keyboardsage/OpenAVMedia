# OpenAVMedia
A subset of open source audio and video media types paired with a convenient build system. 

# Motivation
Open source projects such as vorbis, vpx, etc. utilize several different build systems; such as Makefile, Cmake, Premake, Genie, Ninja, etc.

Spending time trying to configure and compile each project is time consuming.

The purpose of this project is to create a CMake script that will compile them and create a single static library and the necessary headers.