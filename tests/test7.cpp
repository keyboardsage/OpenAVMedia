#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_mixer/SDL_mixer.h>
#include <map>
#include <string>
#include <thread>
#include <chrono>

#define ASSETS_DIR "../../tests/assets/"

// utility functions
// Note: I suspect Mix_PlayChannel is not thread safe and I should not be using it for threading.
// However, since this is small example, I'll use playSound and playFadeInSound and just fudge it anyway.
// Race conditions, catch me if you can, YOLO, LOL.

Mix_Chunk* loadSound(const std::string& file) {
    Mix_Chunk* chunk = Mix_LoadWAV(file.c_str());
    if (!chunk) {
        std::cerr << "Failed to load " << file << ": " << Mix_GetError() << std::endl;
    }
    return chunk;
}

void playSound(Mix_Chunk* sound, int channel = -1, int loops = 0, int delayMs = 0, int volume = MIX_MAX_VOLUME) {
    std::thread([sound, channel, loops, delayMs, volume]() {
        if (delayMs > 0) {
            SDL_Delay(delayMs); // add delay before playing the sound
        }
        Mix_VolumeChunk(sound, volume);
        if (Mix_PlayChannel(channel, sound, loops) == -1) {
            std::cerr << "Failed to play sound: " << Mix_GetError() << std::endl;
        }
    }).detach(); // Run in a detached thread
}

void playFadeInSound(Mix_Chunk* sound, int channel = -1, int loops = 0, int delayMs = 0, int fadeDurationMs = 1000, int volume = MIX_MAX_VOLUME) {
    std::thread([sound, channel, loops, delayMs, fadeDurationMs, volume]() {
        if (delayMs > 0) {
            SDL_Delay(delayMs); // add delay before starting the fade-in
        }

        Mix_VolumeChunk(sound, volume); // Set the maximum volume for the chunk
        if (Mix_FadeInChannel(channel, sound, loops, fadeDurationMs) == -1) {
            std::cerr << "Failed to play sound with fade-in: " << Mix_GetError() << std::endl;
        }
    }).detach(); // Run in a detached thread
}


// plays a soundscape that sounds like a forest in the rain
void playForestScene(const std::map<int, Mix_Chunk*>& sounds) {
    // play 443972 light water stream and 643666/536759 frogs
    playFadeInSound(sounds.at(443972), 0, 0, 0, 1000, 48);
    playSound(sounds.at(643666), 1, 0, 1500);
    playSound(sounds.at(536759), 2, 0, 2000);
    playSound(sounds.at(536759), 2, 0, 3300, 64);

    // play 750670 thunder, 5-second pause, then play faded in 243776/643666 rain and thunder
    playSound(sounds.at(750670), 3, 0, 6000);
    playFadeInSound(sounds.at(243776), 4, 0, 5000, 2000, 48);
    playFadeInSound(sounds.at(536260), 5, 0, 5000, 2000, 48);

    // play 475094 thunder, fade in 454283 faster larger stream
    playSound(sounds.at(475094), 5, 0, 12000);
    playFadeInSound(sounds.at(454283), 6, 0, 6000, 2000, 48);

    playSound(sounds.at(475094), 5, 0, 16000, 82); // another random crack of thunder, but louder by layering via 2 channels
    playSound(sounds.at(475094), 2, 0, 16000, 82);

    // dull playing sounds, then play 451158 water trickling off roof
    Mix_Volume(4, MIX_MAX_VOLUME / 8);
    Mix_Volume(5, MIX_MAX_VOLUME / 8);
    Mix_Volume(6, MIX_MAX_VOLUME / 8);
    playSound(sounds.at(451158), 8, 0, 23000, 96); // After delay, play 451158
    playSound(sounds.at(451158), 2, 0, 23000, 96);
}

std::string chooseAudioDevice() {
    // list the audio devices
    int numDevices = SDL_GetNumAudioDevices(0); // 0 for playback devices
    if (numDevices < 0) {
        std::cerr << "No audio devices found: " << SDL_GetError() << std::endl;
        return "";
    }

    std::cout << "Available playback devices:" << std::endl;
    for (int i = 0; i < numDevices; ++i) {
        std::cout << "  [" << i << "] " << SDL_GetAudioDeviceName(i, 0) << std::endl;
    }

    // choose
    int choice = -1;
    int highestChoice = numDevices - 1;
    while (true) {
        std::cout << "\nEnter the number of your preferred device (0-" << highestChoice << "): ";
        std::cin >> choice;

        if (std::cin.fail()) { // handle invalid input
            std::cin.clear(); // Clear error state
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue;
        }

        if (choice >= 0 && choice <= highestChoice) { // ensure valid input choice is within range
            break; // Valid choice, exit loop
        } else {
            std::cout << "Invalid choice. Please select a number between 0 and " << highestChoice << "." << std::endl;
        }
    }

    return SDL_GetAudioDeviceName(choice, 0);
}

int main(int argc, char* argv[]) {
    // init SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    // choose output device
    std::string selectedDevice = chooseAudioDevice();
    std::cout << "Output device selected: " << selectedDevice << std::endl;

    // init SDL_mixer
    if (Mix_OpenAudioDevice(44100, MIX_DEFAULT_FORMAT, 2, 2048, selectedDevice.c_str(), SDL_AUDIO_ALLOW_ANY_CHANGE) < 0) {
        std::cerr << "SDL_mixer could not initialize: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }
    Mix_AllocateChannels(8); // explicitly allocate 8 channels

    // load sound files into a map
    std::map<int, Mix_Chunk*> sounds = {
        {243776, loadSound(ASSETS_DIR"243776.mp3")},
        {443972, loadSound(ASSETS_DIR"443972.wav")},
        {451158, loadSound(ASSETS_DIR"451158.flac")},
        {454283, loadSound(ASSETS_DIR"454283.flac")},
        {475094, loadSound(ASSETS_DIR"475094.ogg")},
        {536260, loadSound(ASSETS_DIR"536260.opus")},
        {536759, loadSound(ASSETS_DIR"536759.ogg")},
        {643666, loadSound(ASSETS_DIR"643666.mp3")},
        {750670, loadSound(ASSETS_DIR"750670.wav")}
    };

    // verify that all sounds loaded correctly
    for (const auto& [id, chunk] : sounds) {
        if (!chunk) {
            std::cerr << "Failed to load sound ID: " << id << std::endl;
            Mix_CloseAudio();
            SDL_Quit();
            return EXIT_FAILURE;
        }
    }

    // play the forest soundscape
    playForestScene(sounds);
    
    // wait to let sound finish playing
    /*
    while (Mix_Playing(-1) > 0) {
        SDL_Delay(500); // Wait 500ms before checking again to avoid busy-waiting
    }
    */
   SDL_Delay(30000);

    // cleanup
    for (const auto& [_, chunk] : sounds) {
        Mix_FreeChunk(chunk);
    }
    Mix_CloseAudio();
    SDL_Quit();

    return EXIT_SUCCESS;
}
