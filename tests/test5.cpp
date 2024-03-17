#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <deque>

#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "opus/opus.h"
#include "SDL2/SDL.h"
#include "vpx/vpx_codec.h"
#include "soloud/soloud.h"
#include "soloud/soloud_wav.h"

#include "webm/mkvparser/mkvparser.h" // libsimplewebm uses these three headers to playback video
#include "simplewebm/OpusVorbisDecoder.hpp"
#include "simplewebm/VPXDecoder.hpp"

#include "../tests/test5.hpp"

/**
 * @brief This namespace contains a few SDL specific functions that are custom made for handling graphics.
 * @note These functions could have just as easily been done using GLFW and OpenGL, etc.
 */
namespace sdl {
    Uint32 bootstrap_sdl_window(Uint32 width, Uint32 height, SDL_Window*& window, SDL_Renderer*& renderer, SDL_Texture*& texture) {
        // init the video
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return 1;
        }

        // create a window
        window = SDL_CreateWindow("OpenAVMedia Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
        if (window == nullptr) {
            std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
            return 2;
        }

        // add a renderer to the window
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer == nullptr) {
            std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            return 3;
        }

        // add texture to renderer
        // Note: This texture expects YUV data
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, width, height);
        if (texture == nullptr) {
            std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            return 4;
        }

        return 0;
    }

    Uint32 shutdown_sdl_window(SDL_Window*& window, SDL_Renderer*& renderer, SDL_Texture*& texture) {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
    }

    bool handle_sdl_events(SDL_Event* e) {
        // Return true when...
        switch ((*e).type) { // ...the window is closed (clicked X)
            case SDL_QUIT:
                return true;
                break;
            case SDL_KEYDOWN: // ...or espace key is pressed
                if ((*e).key.keysym.sym == SDLK_ESCAPE) return true;
                break;
        }

        return false; // ...otherwise false
    }

    void copy_sdl_texture_to_sdl_renderer(SDL_Renderer*& renderer, SDL_Texture*& texture) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
}

/**
 * --------------------------------------------------------------------------------
 * Below is a custom classes and functions that assist in playback.
 * --------------------------------------------------------------------------------
 */

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
            *available = total_pos;

		return 0;
	}

    private:
	FILE *m_file;
};

/**
 * @brief Calculates the time elapsed since previous function call and this one
 * @param last The previous time
 * @param now The current time
 * @return float that represents the time elapsed between calls
 */
float get_time_delta(int64_t* last, int64_t* now) {
    *last = *now;
    *now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    return static_cast<float>(*now - *last);
}

/**
 * @brief This function increments the frame count
 * @param time_delta The elapsed time since the start of the current frame
 * @return If time_delta exceeds 1 second, frame count is returned. Otherwise, the integer -1 is returned. 
 */
int32_t update_frames_per_second(float time_delta) {
    static float time_delta_accumulator = 0.0f; // Uses two internal variables...
    static int32_t frame_counter = 0;

    // ...to accumulate time and count frames...
    time_delta_accumulator += time_delta;
    frame_counter++;

    // ...and when 1000 milliseconds (1 second) has elapsed...
    if ((1000.0f - time_delta_accumulator) < 1.0f) {
        int32_t return_value = frame_counter;
        
        // ...it resets the internal variables and returns the frames counted over the last second
        time_delta_accumulator = 0.0f;
        frame_counter = 0;
        return return_value;
    }

    return -1; // A negative 1 is returned until ready to provide a frame count update
}

/**
 * @brief Gets the frame count that the webm file was encoded with
 * @param filePath The file path of the webm file
 * @param rate This argument will be set to the frame count
 * @return Zero upon success, otherwise a nonzero error code.
 */
uint32_t webm_frame_rate(const char* filePath, double& rate) {
    // parse the EBML header
    MkvReader reader(filePath);
    mkvparser::EBMLHeader ebmlHeader;
    long long pos = 0;
    if (ebmlHeader.Parse(&reader, pos) < 0) {
        std::cerr << "Error parsing EBML header." << std::endl;
        return 1;
    }

    // create a segment
    mkvparser::Segment* segment = nullptr;
    if (mkvparser::Segment::CreateInstance(&reader, pos, segment) < 0) {
        std::cerr << "Error creating segment instance." << std::endl;
        return 2;
    }
    if (segment->Load() < 0) {
        std::cerr << "Error loading segment." << std::endl;
        return 3;
    }

    // for each...
    const mkvparser::Tracks* tracks = segment->GetTracks();
    for (unsigned long i = 0; i < tracks->GetTracksCount(); ++i) {
        // ...track...
        const mkvparser::Track* track = tracks->GetTrackByIndex(i);
        if (track == nullptr) continue;

        // ...if its a VP8 or VP9 video track...
        if (track->GetType() == mkvparser::Track::kVideo) {
            const mkvparser::VideoTrack* videoTrack = static_cast<const mkvparser::VideoTrack*>(track);

            if (strcmp(videoTrack->GetCodecId(), "V_VP8") == 0 || strcmp(videoTrack->GetCodecId(), "V_VP9") == 0) {
                // ...get the track's...
                if (videoTrack->GetDefaultDuration() > 0) {
                    // ...duration by calculating it based on WebM's default duration...
                    double frameRate = 1000000000.0 / videoTrack->GetDefaultDuration();
                    
                    delete segment;
                    
                    rate = frameRate;
                    return 0; // success
                } else {
                    // ...but if the default duration is not present you will need to find another way
                    // ex: Estimate the frame rate by analyzing frame timestamps
                    std::cerr << "Warning: You may need to find another way to determine frame rate. No default duration present in the current video." << std::endl;
                }
            }
        }
    }

    delete segment;

    std::cerr << "Error: Suitable track/frame rate was not found." << std::endl;
    return 4;
}

const int MILLISECONDS_IN_A_SECOND = 1000;

/**
 * @brief The purpose of this class is to ensure that a loop iterates a target number of times.
 */
class FrameRegulator {
    public:
    FrameRegulator(int target_fps) {
        targetFPS(target_fps);

        m_targetFrameDuration = std::round(MILLISECONDS_IN_A_SECOND / target_fps);
    }
    ~FrameRegulator() { };

    void start() {
        m_frameStart = std::chrono::steady_clock::now();
    }
    void stop() {
        m_frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_frameStart).count();
    }
    void delay() {
        //DEBUG: std::cout << "Waiting..." << (m_targetFrameDuration - m_frameTime) << " ms because " << m_frameTime << " ms have passed" << std::endl;
        if (m_frameTime < m_targetFrameDuration) {
            std::this_thread::sleep_for(std::chrono::milliseconds(m_targetFrameDuration - m_frameTime));
        } else {
            std::cerr << "Warning: Running a little slow. No waiting was required this frame/iteration." << std::endl;
        }
    }

    void targetFPS(int target_fps) {
        m_targetFPS = target_fps;
    }

    private:
    std::chrono::_V2::steady_clock::time_point m_frameStart; // start time of the frame
    std::chrono::milliseconds::rep m_frameTime;              // elapsed time between the start() and stop(), the frame processing time
    int m_targetFPS;                // target number of frames-per-second
    uint64_t m_targetFrameDuration; // duration in milliseconds
};

/**
 * --------------------------------------------------------------------------------
 * Main
 * --------------------------------------------------------------------------------
 */

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Requires a single argument that contains the video file's file path." << std::endl;
        return EXIT_FAILURE;
    }

    // get video information needed to setup the and play the video
    WebMDemuxer demuxer(new MkvReader(argv[1]));
    if (demuxer.isOpen()) {
    } else {
        std::cerr << "Failed to create WebMDemuxer: Unable to open " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    int video_width = demuxer.getWidth();
    int video_height = demuxer.getHeight();

    double frame_rate = 0.0;
    if (webm_frame_rate(argv[1], frame_rate) != 0) return EXIT_FAILURE;

    // status message
    std::cout << "Play File:    " << argv[1] << "\nVideo Length: " << demuxer.getLength() << "\nFrame Rate: " << frame_rate << std::endl;

    // get a SDL window open
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    if (sdl::bootstrap_sdl_window(video_width, video_height, window, renderer, texture) != 0) {
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // creating variables prior to the loop, so they aren't created repeatedly per iteration
    bool is_user_quitting = false;        // controls when to quit running the app

	int64_t last = 0;                     // these 3 track the amount of time that elapses between iterations
    int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    float delta = 0.0f;
    float accumulated_delta = 0.0f;

    int32_t frame_count = 0;                   // stores frame count over last second
    FrameRegulator frameRegulator(frame_rate); // utilized to reach the playback frame rate goal

    SDL_Event e;                         // SDL's structure for tracking input

    VPXDecoder videoDec(demuxer, 8);     // interfaces for video codecs
    OpusVorbisDecoder audioDec(demuxer);

    CustomAudioSource customSource;      // get SoLoud initialized
    customSource.mChannels = demuxer.getChannels();
    customSource.mBaseSamplerate = demuxer.getSampleRate();
    SoLoud::Soloud soloud;
    soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::AUTO, demuxer.getSampleRate(), 0, demuxer.getChannels());
    SoLoud::handle soundHandle;

    WebMFrame videoFrame, audioFrame;    // two frame types and two buffers for manipulating the data within them
    VPXDecoder::Image image;
    short* pcm = audioDec.isOpen() ? new short[audioDec.getBufferSamples() * demuxer.getChannels()] : nullptr;

    // status
    std::cout << "Audio deque/buffer size: " << customSource.audioBuffer.size()
        << "\nSoloud Global Samplerate: " << soloud.mSamplerate
        << "\nSoloud Global Buffer Size: " << soloud.mBufferSize << std::endl;

    // loop for playing the video
	while ((!is_user_quitting) && demuxer.readFrame(&videoFrame, &audioFrame)) {
        frameRegulator.start(); // consider this the start of the frame

        // get latest input events
        SDL_PollEvent(&e);
        is_user_quitting = sdl::handle_sdl_events(&e); // process them

        // delta is the time that has elapsed since last update_frame_rate() call
        delta = get_time_delta(&last, &now);

        // update the playback position
        accumulated_delta += delta;
        
        // update the frame count
        frame_count = update_frames_per_second(delta);
        if (frame_count != -1) std::cout << "Video Frames Per Second: " << frame_count << std::endl;

        // get the next video frame based on playback position and put its pixels into a texture
        if (videoDec.isOpen() && videoFrame.isValid()) {
            if (!videoDec.decode(videoFrame))
            {
                std::cerr << "Failed to decode video frame. Shutting down..." << std::endl;
                soloud.deinit();
                sdl::shutdown_sdl_window(window, renderer, texture);
                return EXIT_FAILURE;
            }
            while (videoDec.getImage(image) == VPXDecoder::NO_ERROR)
            {
                // For each color plane...
                for (int p = 0; p < 3; ++p) {
                    // ...determine the dimensions
                    // Note: This can be necessary because for some formats, e.g., YUV, the format contains planes with differing dimensions.
                    const int w = image.getWidth(p);
                    const int h = image.getHeight(p);

                    // ...then copy each row of the current plane from the decoder's color plane to the Image's color plane
                    int offset = 0;
                    for (int y = 0; y < h; ++y) {
                        offset += image.linesize[p];
                    }
                }

                // copy the Image data to the SDL texture by...
                // ...updating the texture with YUV frame data
                if (SDL_UpdateYUVTexture(texture, NULL,
                                    image.planes[0], image.linesize[0],           // Y plane
                                    image.planes[1], image.linesize[1],           // U (Cb) plane
                                    image.planes[2], image.linesize[2]) == -1) {  // V (Cr) plane
                    std::cerr << "Unable to update the texture with YUV data: " << SDL_GetError() << std::endl;
                    soloud.deinit();
                    sdl::shutdown_sdl_window(window, renderer, texture);
                    return EXIT_FAILURE;
                }

                // ...and then rendering this texture, SDL will handle the YUV to RGB conversion internally
                if (SDL_RenderCopy(renderer, texture, NULL, NULL) < 0) {
                    std::cerr << "Unable to update render target with the latest texture: " << SDL_GetError() << std::endl;
                    soloud.deinit();
                    sdl::shutdown_sdl_window(window, renderer, texture);
                    return EXIT_FAILURE;
                }
            }
            
            //DEBUG: std::cout << "videoFrame: " << videoFrame.time << " progress: " << (accumulated_delta/MILLISECONDS_IN_A_SECOND) << std::endl;
        }

        // get the next video frame based on playback position and put its pixels into a texture
        if (audioDec.isOpen() && audioFrame.isValid())
        {
            int numOutSamples;
            if (!audioDec.getPCMS16(audioFrame, pcm, numOutSamples))
            {
                std::cerr << "Failed to decode audio frame. Shutting down..." << std::endl;
                soloud.deinit();
                sdl::shutdown_sdl_window(window, renderer, texture);
                return EXIT_FAILURE;
            }

            /*
            // DEBUG: Printing the raw audio out to a file to check/hear it in Audacity software
            FILE* descriptor = fopen("./theraw", "ab");
            fwrite(pcm, 1, numOutSamples * demuxer.getChannels() * sizeof(short), descriptor);
            fclose(descriptor);
            */
           
            // Push the decoded samples into the buffer.
            for (int i = 0; i < (numOutSamples * demuxer.getChannels()); i++) {
                customSource.audioBuffer.push_back(pcm[i]);
            }
            
            // ensure playback can't repeat and play
            if (!soloud.isValidVoiceHandle(soundHandle) || !soloud.getVoiceCount()) {
                soundHandle = soloud.play(customSource);
            }
        }

        // render the texture
        sdl::copy_sdl_texture_to_sdl_renderer(renderer, texture);

        frameRegulator.stop();  // consider this the end of the frame

        // pace the video by performing...
        if (videoFrame.time < (accumulated_delta/MILLISECONDS_IN_A_SECOND)) {
            // ...no operation (nop) anytime the video is running behind
        } else {
            // ...or adding delays to ensure its playback speed matches the goal frame rate when running ahead
            frameRegulator.delay();
        }
    }

    // clean up
    delete[] pcm;
    soloud.deinit();
    sdl::shutdown_sdl_window(window, renderer, texture);

    return EXIT_SUCCESS;
}