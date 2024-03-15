#include <iostream>
#include <vector>
#include <fstream>
#include <SDL2/SDL.h>

#include "vpx/vpx_codec.h"

#include "webm/mkvparser/mkvparser.h" // libsimplewebm uses these three headers to playback video
#include "simplewebm/OpusVorbisDecoder.hpp"
#include "simplewebm/VPXDecoder.hpp"

/**
 * @brief Matroska parser class inherited from WebM library
 * @note WebM containers are a derivative of Matroska
 */
class MkvReader: public mkvparser::IMkvReader {
    public:
	MkvReader(const char* filePath): m_file(fopen(filePath, "rb")) { }
	~MkvReader() {
        if (m_file) fclose(m_file);
    }

	int Read(long long pos, long len, unsigned char* buf)
	{
		if (!m_file) return -1; // if file descriptor/handle is valid...

        // ...then start at `pos` and read `len` bytes into `buf`
		fseek(m_file, pos, SEEK_SET);
		const size_t size = fread(buf, 1, len, m_file);
		
		return (size < size_t(len)) ? -1 : 0; // returns -1 if unable to read len bytes, otherwise zero
	}
	
    int Length(long long* total, long long* available)
	{
		if (!m_file) return -1; // if file descriptor/handle is valid...

		const off_t curr_pos = ftell(m_file);  // ...get current position
		fseek(m_file, 0, SEEK_END);
        const off_t total_pos = ftell(m_file); // ...and total file length
        fseek(m_file, curr_pos, SEEK_SET); // maintain the file's original position
		
        // update the parameters
        if (total)
			*total = total_pos;
		if (available)
			//*available = total_pos - curr_pos; // TODO: this seems logical but it causes the video playback to fail
            *available = total_pos;

		return 0;
	}

    private:
	FILE *m_file;
};

/**
 * @brief Calculate the audio duration in seconds
 * @param audioBuffer A vector consisting of 16-bit signed integers representing the audio data
 * @param sampleRate The audio's sample rate
 * @param numChannels The number of channels encoded by the audio
 * @return double representing the media's length in seconds
 */
double calculateDuration(const std::vector<short>& audioBuffer, int sampleRate, int numChannels) {
    // total samples in the buffer = buffer size / number of channels
    size_t totalSamples = audioBuffer.size() / numChannels;

    // duration in seconds = total samples / sample rate
    double durationInSeconds = static_cast<double>(totalSamples) / sampleRate;

    return durationInSeconds;
}

// GLOBALS
static Uint8 *audio_playback_pos;  // audio position as a pointer, it points to a point within the audio buffer
static Uint64 audio_remaining_len; // remaining length, in bytes, of the audio we have to play

// CALLBACK
void SDLCALL audio_callback(void *userdata, Uint8 *stream, int len) {
    //DEBUG:
    //static int callbackCount = 0;
    //std::cout << "Callback #" << ++callbackCount << " - Requested Length: " << len << std::endl;

    if (audio_remaining_len == 0) {    // do nothing if there is nothing to do
        SDL_memset(stream, '\0', len); // just play silence
        return;
    }

    len = (len > audio_remaining_len) ? audio_remaining_len : len;

    SDL_memcpy(stream, audio_playback_pos, len); // copy audio data
    
    audio_playback_pos += len; // update
    audio_remaining_len -= len;

    //DEBUG: std::cout << "Callback #" << callbackCount << " - Position: " << (void*)audio_playback_pos << " - Remaining: " << audio_remaining_len << std::endl;
}

// MAIN
int main(int argc, char *argv[]) {
    // sanity check
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>.webm" << std::endl;
        return EXIT_FAILURE;
    }

    // init SDL and setup libsimplewebm's demultiplexer
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    atexit(SDL_Quit);

    WebMDemuxer demuxer(new MkvReader(argv[1]));
    if (!demuxer.isOpen()) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // make audio device with the specification we want and we WILL get
    std::vector<short> audioBuffer;
    SDL_AudioSpec want;
    SDL_memset(&want, 0, sizeof(want));

    want.freq = demuxer.getSampleRate();   // match media's sample rate
    want.format = AUDIO_S16;               // libsimplewebm always returns signed 16-bit audio
    want.channels = demuxer.getChannels(); // match media's channel count
    want.samples = 4096;                   // 4096 is a good size for most standard applications 
    want.callback = audio_callback;        // function for consuming audio data
    want.userdata = &audioBuffer;          // audio data to consume

    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);// zero in 4th param makes the spec mandatory
    if (audioDevice == 0) {
        std::cerr << "SDL audio bootstrapping failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // After opening the audio device, check what specifications we actually got
    std::cout << "format:    " << want.format << std::endl;
    std::cout << "frequency: " << want.freq << std::endl;
    std::cout << "channels:  " << static_cast<int>(want.channels) << std::endl;
    std::cout << "samples:   " << want.samples << std::endl;

// ------------------------------------------------------------------------------------------------


    // all initialization is done, now we must decode the audio from the webm file
    OpusVorbisDecoder preAudioDec(demuxer);
    WebMFrame audioFrame;

    short *pcm = new short[preAudioDec.getBufferSamples() * demuxer.getChannels()];
    
    while (demuxer.readFrame(NULL, &audioFrame)) {
        if (preAudioDec.isOpen() && audioFrame.isValid()) {
            // suck up all the audio data
            int numOutSamples;
            if (!preAudioDec.getPCMS16(audioFrame, pcm, numOutSamples)) {
                std::cerr << "Failed to decode audio frame." << std::endl;
                SDL_CloseAudioDevice(audioDevice);
                SDL_Quit();
                return EXIT_FAILURE;
            }

            // and put it in the audioBuffer vector
            for (int i = 0; i < (numOutSamples * demuxer.getChannels()); i++) {
                audioBuffer.push_back(pcm[i]);
            }
        }
    }

    delete[] pcm;
    
    /*
    // DEBUG: print out the audiobuffer so it can be checked/loaded in Audacity
    std::ofstream outFile("output_audio.raw", std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(reinterpret_cast<char*>(audioBuffer.data()), audioBuffer.size() * sizeof(short));
        outFile.close();
        std::cout << "Output audio saved." << std::endl;
    } else {
        std::cerr << "Failed to open output_audio.raw for writing." << std::endl;
    }
    */

    // setup global playback parameters
    audio_playback_pos = (Uint8*)(audioBuffer.data());        // playback will start from the beginning of the buffer
    audio_remaining_len = audioBuffer.size() * sizeof(short); // remaining bytes to play

    std::cout << "Playback starting from:  " << static_cast<void*>(audio_playback_pos) << std::endl;
    std::cout << "Uncompressed audio size: " << audio_remaining_len << " bytes" << std::endl;
    
    // Calculate and print the duration
    double duration = calculateDuration(audioBuffer, demuxer.getSampleRate(), demuxer.getChannels());
    std::cout << "Expected audio duration: " << duration << " seconds" << std::endl;


// ------------------------------------------------------------------------------------------------


    // start playing
    SDL_PauseAudioDevice(audioDevice, 0);

    // main loop
    bool is_user_quitting = false;
    SDL_Event e;
    while (!is_user_quitting) {
        // quit if necessary
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                is_user_quitting = true;
            }
        }

        SDL_Delay(100);  // simulates work
    }

    // clean up
    SDL_CloseAudioDevice(audioDevice);
    SDL_Quit();

    return EXIT_SUCCESS;
}