#include <iostream>
//#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "opus/opus.h"

int main() {
    //const char* oggVersion = ogg_version_string();
    //std::cout << "Linked libogg version: " << std::endl;

    const char* vorbis_version = vorbis_version_string();
    std::cout << "Linked libvorbis version: " << vorbis_version << std::endl;

    const char* opus_version = opus_get_version_string();
    std::cout << "Linked libopus version:   " << opus_version << std::endl;

    // SDL version
    /*if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_version compiled;
    SDL_VERSION(&compiled);
    std::cout << "SDL version compiled: "
        << static_cast<int>(compiled.major) << "."
        << static_cast<int>(compiled.minor) << "."
        << static_cast<int>(compiled.patch) << std::endl;

    SDL_Quit();*/

    return EXIT_SUCCESS;
}