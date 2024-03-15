#pragma once
#include <deque>

#include "soloud/soloud.h"


// prototype for CustomAudioSourceInstance
/**
 * @brief Custom audio source instance
 * @note The SoLoud library requires that this CustomAudioSourceInstance class be defined first so
 * that CustomAudioSource can be defined later. 
 */
class CustomAudioSource;
class CustomAudioSourceInstance: public SoLoud::AudioSourceInstance {
public:
    CustomAudioSource* mParentSource;

    explicit CustomAudioSourceInstance(CustomAudioSource* aSource);

    virtual unsigned int getAudio(float* aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize) override;

    virtual bool hasEnded() override;
};

/**
 * @brief Custom audio source
 * @note The SoLoud library requires that this CustomAudioSource class define the createInstance().
 */
class CustomAudioSource: public SoLoud::AudioSource {
    public:
    std::deque<short> audioBuffer;     // audio buffer (for simplicity, public access)

    CustomAudioSource() {
        this->mChannels = 1;           // starts mono
        this->mBaseSamplerate = 44100; // starts 44100 Hz
    }
    virtual ~CustomAudioSource() noexcept {}

    virtual SoLoud::AudioSourceInstance* createInstance()
    {
        return new CustomAudioSourceInstance(this);
    }
};


CustomAudioSourceInstance::CustomAudioSourceInstance(CustomAudioSource* aSource): SoLoud::AudioSourceInstance(), mParentSource(aSource)
{
}

unsigned int CustomAudioSourceInstance::getAudio(float* aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize)
{
    unsigned int samplesWritten = 0;
    for (unsigned int i = 0; i < aSamplesToRead; ++i, ++samplesWritten)
    {
        // Mono audio
        if (mParentSource->mChannels == 1) {
            if (!mParentSource->audioBuffer.empty()) {
                short sample = mParentSource->audioBuffer.front();
                mParentSource->audioBuffer.pop_front();
                aBuffer[i] = sample / 32768.0f;
            } else {
                aBuffer[i] = 0.0f;
            }
        }

        // Stereo audio
        else if (mParentSource->mChannels == 2) {
            if (mParentSource->audioBuffer.size() >= 2) { // check if we have at least two samples (one for each channel)
                short sampleLeft = mParentSource->audioBuffer.front();
                mParentSource->audioBuffer.pop_front();
                short sampleRight = mParentSource->audioBuffer.front();
                mParentSource->audioBuffer.pop_front();
                
                // normalize the samples to the range of -1.0 to 1.0
                // also add some headroom, to prevent clipping
                // left channel is first half of the buffer, right channel is second half of the buffer
                aBuffer[i]     = (sampleLeft / 32768.0f)*0.95;            // left channel
                aBuffer[i + aBufferSize] = (sampleRight / 32768.0f)*0.95; // right channel
            } else {
                // DEBUG: std::cout << "Lack of Data - aSamplesToRead: " << aSamplesToRead << " - aBufferSize:" << aBufferSize << std::endl;
                aBuffer[i] = aBuffer[i + aBufferSize] = 0.0f; // when there is not enough data for stereo, output silence
            }
        }
    }

    /*
    // DEBUG: 
    std::cout << "aBufferSize: " << aBufferSize 
        << " - mParentSource->audioBuffer.size(): " << mParentSource->audioBuffer.size()
        << " - samplesWritten: " << samplesWritten << std::endl;
    std::cout << "Consuming " << aSamplesToRead << " samples from buffer. Remaining size: " << mParentSource->audioBuffer.size() << std::endl;
    */

    return samplesWritten;
}
bool CustomAudioSourceInstance::hasEnded()
{
    // stream ends when there are no more samples left to play
    return mParentSource->audioBuffer.empty();
}