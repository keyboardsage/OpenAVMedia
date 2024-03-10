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
    for (unsigned int i = 0; i < aSamplesToRead; i += mParentSource->mChannels)
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
                aBuffer[i] = sampleLeft / 32768.0f; // left channel
                aBuffer[i + 1] = sampleRight / 32768.0f; // right channel
                //DEBUG: std::cout << "i: " << i << " i+1: " << i + 1 << " aBufferSize: " << aBufferSize << std::endl;
            } else {
                //DEBUG: std::cout << "Lack of Data" << std::endl;
                aBuffer[i] = aBuffer[i + 1] = 0.0f; // when there is not enough data for stereo, output silence
            }
        }

        samplesWritten += mParentSource->mChannels;
    }

    //DEBUG: std::cout << "aBufferSize: " << aBufferSize << " mParentSource->audioBuffer.size(): " << mParentSource->audioBuffer.size() << std::endl;
    
    return samplesWritten;
}

bool CustomAudioSourceInstance::hasEnded()
{
    // stream ends when there are no more samples left to play
    return mParentSource->audioBuffer.empty();
}