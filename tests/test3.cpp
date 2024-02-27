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
 */
class MkvReader: public mkvparser::IMkvReader {
    public:
	MkvReader(const char* filePath): m_file(fopen(filePath, "rb")) { }
	~MkvReader() {
        if (m_file) fclose(m_file);
    }

	int read(long long pos, long len, unsigned char* buf)
	{
		if (!m_file) return -1; // if file descriptor/handle is valid...

        // ...then start at `pos` and read `len` bytes into `buf`
		fseek(m_file, pos, SEEK_SET);
		const size_t size = fread(buf, 1, len, m_file);
		
		return (size < size_t(len)) ? -1 : 0; // returns -1 if unable to read len bytes, otherwise zero
	}
	
    int length(long long* total, long long* available)
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

int main(int argc, char* argv[]) {
    if (argc != 2) return EXIT_FAILURE;

    // SDL skeleton
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

