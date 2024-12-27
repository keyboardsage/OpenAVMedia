#include <cstdlib>
#include <iostream>
#include <string>
#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "opus/opus.h"
#include "FLAC/all.h"
#include "opus/opus_multistream.h"
#include "opusfile/opusfile.h"
#include "SDL_mixer/SDL_mixer.h"
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

    std::cout << "libFLAC version:               " << FLAC__VERSION_STRING << std::endl;

    std::string opusfile_version = "Unknown";
    std::cout << "Linked libopusfile version:    " << opusfile_version.c_str() << std::endl; // TODO: no straight-forward way to get this information

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

    // SDL_mixer version
    SDL_version sdl_mixer_compiled;
    SDL_MIXER_VERSION(&sdl_mixer_compiled);
    std::cout << "SDL_mixer compiled version:    "
              << static_cast<int>(sdl_mixer_compiled.major) << "."
              << static_cast<int>(sdl_mixer_compiled.minor) << "."
              << static_cast<int>(sdl_mixer_compiled.patch) << std::endl;

    std::string simplewebm_version = "Unknown";
    std::cout << "Linked libsimplewebm version:  " << simplewebm_version.c_str() << std::endl; // TODO: no straight-forward way to get this information

    return EXIT_SUCCESS;
}