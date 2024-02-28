#include <cstdlib>
#include <iostream>
#include <string>

#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "opus/opus.h"
#include "SDL2/SDL.h"
#include "vpx/vpx_codec.h"
#include "soloud/soloud.h"

#include "webm/mkvparser/mkvparser.h" // libsimplewebm use these three headers to playback video
#include "simplewebm/OpusVorbisDecoder.hpp"
#include "simplewebm/VPXDecoder.hpp"

/**
 * @brief Matroska parser class derived from WebM library
 * @note Remember that the WebM container is a derivative of Matroska. So don't be surprised by the naming of this class.
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
			*available = total_pos - curr_pos;

		return 0;
	}

    private:
	FILE *m_file;
};

/**
 * @brief A few SDL specific functions for handling graphics.
 * @note These functions could have just as easily been done using GLFW and OpenGL, etc.
 */
namespace sdl {
    Uint32 bootstrap_sdl_window(Uint32 width, Uint32 height, SDL_Window* window, SDL_Renderer* renderer,SDL_Texture* texture) {
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
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) {
            std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            return 3;
        }

        // add texture to renderer
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);
        if (texture == nullptr) {
            std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            return 4;
        }

        return 0;
    }

    Uint32 shutdown_sdl_window(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture) {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
    }

    float get_sdl_time_delta(Uint32* last, Uint32* now) {
        *last = *now;
        *now = SDL_GetTicks();

        return (float)(*now - *last); // time elapsed since previous function call and this one
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

    void copy_sdl_texture_to_sdl_renderer(SDL_Renderer* renderer, SDL_Texture* texture) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
}

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

int main(int argc, char* argv[]) {
    if (argc != 2) return EXIT_FAILURE;

    // get video information needed to setup the window
    int video_width = 640;
    int video_height = 360;
    int pitch; // this is the length of one row of pixels, in bytes

    // get a SDL window open
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    if (sdl::bootstrap_sdl_window(video_width, video_height, window, renderer, texture) != 0) {
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // creating variables prior to the loop, so they aren't created repeatedly per iteration
    bool end_loop = false;

	Uint32 last = 0;
    Uint32 now = SDL_GetTicks();
    float delta = 0.0f;

    int32_t frame_count = 0;

    SDL_Event e;

	while (!end_loop) {
        SDL_PollEvent(&e); // get latest events
        end_loop = sdl::handle_sdl_events(&e); // process them

        // delta is the time that has elapsed since last update_frame_rate() call
        delta = sdl::get_sdl_time_delta(&last, &now);

        // update the frames that were played over the last second
        frame_count = update_frames_per_second(delta);
        if (frame_count != -1) std::cout << "Frames Per Second: " << frame_count << std::endl;

        // update the playback position

        // get the next video frame based on playback position and put its pixels into a texture
        /*auto frame = clip->getNextFrame();
		if (frame != nullptr) {
			void *pixels;

			SDL_LockTexture(texture, nullptr, &pixels, &pitch);
			memcpy(pixels, frame->getBuffer(), sizeof(Uint8) * pitch * frame->getHeight());
			SDL_UnlockTexture(texture);

			clip->popFrame();
		}*/

        // render the texture
        sdl::copy_sdl_texture_to_sdl_renderer(renderer, texture);

        SDL_Delay(20); // maximum of 50 frames-per-second. A small delay prevents excessive CPU utilization.
    }

    // clean up
    sdl::shutdown_sdl_window(window, renderer, texture);

    return EXIT_SUCCESS;
}