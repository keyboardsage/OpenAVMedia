#!/bin/bash

# Prep
mkdir temp
cp libs/*.a temp/
cd temp

# Step 1: Extract object files from static libraries
ar -x libogg.a
ar -x libopus.a
ar -x libSDL2.a
ar -x libSDL2main.a
ar -x libSDL2_test.a
ar -x libsoloud.a
ar -x libvorbis.a
ar -x libvpx.a
ar -x libwebm.a

# Step 2: Combine into single object file
#ld -r -o combined.o *.o

# Step 3: Create new static library
ar rcs libopenavmedia.a *.o # Directly archive all object files into libopenavmedia.a
#ar rcs libopenavmedia.a combined.o
cp libopenavmedia.a ../libs/
sleep 2

# Step 4: Clean up
cd ..
rm -rf temp

if [ ! -f libs/libopenavmedia.a ]; then
    echo "Error: libopenavmedia.a does not exist."
    exit 1
fi
echo -e "\033[1;33mlibopenavmedia.a was created successfully.\033[1;0m"