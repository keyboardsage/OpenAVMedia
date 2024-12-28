#!/bin/bash

# Prep
mkdir temp
mkdir temp_opus
mkdir temp_vorbis
mkdir temp_flac
cp libs/*.a temp/
cp libs/libopus.a temp_opus/
cp libs/libvorbis.a temp_vorbis/
cp libs/libFLAC.a temp_flac/

# Step 1: Extract object files from static libraries
cd temp_opus;ar -x libopus.a;cd ..
cd temp_vorbis;ar -x libvorbis.a;cd ..
cd temp_flac;ar -x libFLAC.a;cd ..
cd temp
ar -x libSDL2.a
ar -x libSDL2main.a
ar -x libSDL2_test.a
ar -x libsoloud.a
ar -x libvorbisenc.a
ar -x libvorbisfile.a
ar -x libopusfile.a
ar -x libSDL2_mixer.a
ar -x libogg.a
ar -x libsimplewebm.a
ar -x libvpx.a
ar -x libwebm.a

# Step 2: Combine into single object file
#ld -r -o combined.o *.o

# Step 3: Create new static library
ar rcs libopenavmedia.a *.o ../temp_opus/*.o ../temp_vorbis/*.o ../temp_flac/*.o # Directly archive all object files into libopenavmedia.a
#ar rcs libopenavmedia.a combined.o
#ranlib libopenavmedia.a
cp libopenavmedia.a ../libs/
sleep 2

# Step 4: Clean up
cd ..
rm -rf temp
rm -rf temp_opus
rm -rf temp_vorbis
rm -rf temp_flac

if [ ! -f libs/libopenavmedia.a ]; then
    echo "Error: libopenavmedia.a does not exist."
    exit 1
fi
echo -e "\033[1;33mlibopenavmedia.a was created successfully.\033[1;0m"