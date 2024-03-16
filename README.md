# OpenAVMedia
Some open source audio and video media libraries, a convenient build system, and a few examples. 

This project compiles several open source audio/video projects and produces a single directory filled with the necessary static libraries and headers for playing back `.webm` video files.

Additionally, it contains example source code showing how to use the aforementioned open source projects to playback `.webm` media in the `tests` directory.

# Current State
Working Audio and Video playback:
`tests/test5` and `tests/test6` demonstrate how to utilize the libraries to make a simple WEBM video player.
  - It plays VP8/Vorbis ([download page](https://www.webmfiles.org/demo-files/))
  - It plays VP9/Opus   ([download page](https://commons.wikimedia.org/wiki/File:WING_IT!_-_Blender_Open_Movie-full_movie.webm))
  - Videos must be encoded with mono or stereo audio

<p align="center">
<img src="test5_running.gif" alt="Test5 running">
</p>

# Supported Platforms
  - [X] Linux
  - [ ] Mac
  - [ ] Windows

# Building
Go into the build directory. Configure and create the project like so:
```
cd build
cmake ..
make
```
The static file for the libraries built will be under `build/libs` and their include files will be under `build/libs/include`. There is also one additional file called `libopenavmedia.a`, this is all the other static libraries combined.

# Running
Go into the build directory and run them like so:
```
$ cd build

$ ./tests/test1
$ ./tests/test2

$ ./tests/test3 ../tests/assets/big-buck-bunny_trailer.webm
$ ./tests/test4 ../tests/assets/big-buck-bunny_trailer.webm

$ ./tests/test5 ../tests/assets/WING_IT-Blender_Open_Movie-full_movie.webm
$ ./tests/test6 ../tests/assets/WING_IT-Blender_Open_Movie-full_movie.webm
```

The purpose of each example is as follows:  
`test1` - Shows the version of the statically compiled libraries by linking to each one individually.  
`test2` - Same as test1 but links only against libopenavmedia.  

`test3` - Plays the audio portion of a webm file, statically linked to each library again.  
`test4` - Same as test3 but links only against libopenavmedia.a  

`test5` - Plays a webm video file, statically linked to each library.  
`test6` - Same as test5 but links only against libopenavmedia.a  

# Cleaning
Go into the build directory and run these commands.
```
make clean
make clean-libs
```

# Motivation
While developing a game engine I needed cutscene functionality. This forced me to evaluate the licenses of libav and VLC; which didn't suit my purposes. In brief:

- libav: Uses GPL and LGPL code. Static linking with its GPL components requires releasing your source code, this can be avoided by dynamically linking only LGPL parts, which necessitates including those libraries when you deploy.
- VLC: Its legal complexities are nuanced, stemming from software patent laws that differ between the US and EU. VideoLan's [legal page](https://www.videolan.org/legal.html) notes that EU law doesn't recognize software as patentable, potentially complicating H.264/H.265 and the distribution of other codecs.

So, I sought to cobble together a video player using various open source audio/video decoders.

Firstly, the are a plethora of build systems in use by open source projects like vorbis, vpx, etc.; namely Make, Cmake, Premake, GENie, to name a few. This project uses the CPM extension to download the dependencies for you and consolidate them under a single CMake build system. This prevents you from having to do the time-consuming busy work of configuring and compiling each project separately.  
Secondly, Cmake produces statically linkable files. You get the benefits of a static library (see "Why a Static Library" section for details).  

TLDR; I aimed to find a simple, static library for easy video playback that minimizes dependencies, compiles smoothly, and is safe for commercial use without legal worries.

# Why a Static Library
In theory, different versions of the same dynamic library should work. However, in practice, your program may break due to the nature of the changes between them.

Best practice is to extensively test multiple versions of the same dynamic library. Alternatively, you can restrict the dynamic libraries to certain versions; ones you know perform correctly. However, if you take this latter approach, you are practically using a static library anyway.

Moreover, In the context of a game engine, here are some considerations:
1. Only one game instance runs at a time, singly, which makes the advantage of sharing the library in memory irrelevant.
2. Consistent library versions across all players helps prevent giving any one person an unfair advantage.
3. Dynamic libraries pose additional security risks, like LD_PRELOAD exploits and other linker/loader attacks.
4. Game updates provide control over dependencies, ensuring game developers, not end-users, manage and maintain the dependency code's uniformity.

Put simply, why static, its a design decision that prevents versioning issues, helps game engines maintain fairness across installations, and it suits this particular scenario.

## Libraries Overview
Libraries checked in the Included column below are in use by the program. Libraries checked in the Considering column may get included in the future.

Library | URL | Included | Considering
--- | --- | :---: | :---:
ogg | [https://github.com/xiph/ogg](https://github.com/xiph/ogg) | ✅ | 
vorbis | [https://github.com/xiph/vorbis](https://github.com/xiph/vorbis) | ✅ | 
opus | [https://github.com/xiph/opus](https://github.com/xiph/opus) | ✅ | 
webm | [https://github.com/webmproject/libwebm](https://github.com/webmproject/libwebm) | ✅ | 
vpx | [https://github.com/webmproject/libvpx](https://github.com/webmproject/libvpx) | ✅ | 
sdl2 | [https://github.com/libsdl-org/SDL](https://github.com/libsdl-org/SDL) | ✅ | 
soloud | [https://github.com/jarikomppa/soloud](https://github.com/jarikomppa/soloud) | ✅ | 
libsimplewebm | [https://github.com/zaps166/libsimplewebm](https://github.com/zaps166/libsimplewebm) | ✅ | 
daala | [https://github.com/xiph/daala](https://github.com/xiph/daala) |  | ✅
flac | [https://github.com/xiph/flac](https://github.com/xiph/flac) |  | ✅
xspf | [https://sourceforge.net/projects/libspiff/](https://sourceforge.net/projects/libspiff/) |  | ✅
qoa | [https://github.com/phoboslab/qoa](https://github.com/phoboslab/qoa) |  | ✅


## Video Licenses

This project includes the following videos:

1. "Big Buck Bunny" by Blender Foundation is licensed under [CC BY 3.0](https://creativecommons.org/licenses/by/3.0/). Source: [https://peach.blender.org/](https://peach.blender.org/).

2. "WING IT!" by Blender Foundation is licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/). Source: [https://commons.wikimedia.org/wiki/File:WING_IT!_-_Blender_Open_Movie-full_movie.webm](https://commons.wikimedia.org/wiki/File:WING_IT!_-_Blender_Open_Movie-full_movie.webm).