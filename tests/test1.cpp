#include <iostream>
#include <string>
#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "opus/opus.h"
#include "SDL2/SDL.h"
#include "vpx/vpx_codec.h"
#include "soloud/soloud.h"

int main() {
    const char* ogg_version = ogg_version_string();
    std::cout << "Linked libogg version:         " << ogg_version << std::endl;

    const char* vorbis_version = vorbis_version_string();
    std::cout << "Linked libvorbis version:      " << vorbis_version << std::endl;

    const char* opus_version = opus_get_version_string();
    std::cout << "Linked libopus version:        " << opus_version << std::endl;

    std::string webm_version = "Unknown";
    std::cout << "Linked libwebm version:        " << webm_version.c_str() << std::endl; // TODO: no straight-forward way to get this information

    const char* vpx_version = vpx_codec_version_str();
    std::cout << "Linked libvpx version:         " << vpx_version << std::endl;

    SoLoud::Soloud soloud;
    std::cout << "Linked SoLoud version:         " << soloud.getVersion() << std::endl;

    // SDL version
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_version compiled;
    SDL_VERSION(&compiled);
    std::cout << "Linked SDL version:            "
        << static_cast<int>(compiled.major) << "."
        << static_cast<int>(compiled.minor) << "."
        << static_cast<int>(compiled.patch) << std::endl;

    SDL_Quit();

    return EXIT_SUCCESS;
}