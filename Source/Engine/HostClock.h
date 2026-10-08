#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class HostClock
{
public:
    void update(juce::AudioPlayHead* playHead, double sampleRate, int numSamples)
    {
        currentSampleRate = sampleRate;

        if (playHead != nullptr)
        {
            if (auto posInfo = playHead->getPosition())
            {
                if (posInfo->getBpm())
                {
                    bpm = *posInfo->getBpm();
                    usingHostTempo = true;
                }

                if (posInfo->getIsPlaying())
                    isPlaying = true;
                else
                    isPlaying = false;

                if (posInfo->getTimeInSamples())
                    timeInSamples = *posInfo->getTimeInSamples();

                if (posInfo->getPpqPosition())
                    ppqPosition = *posInfo->getPpqPosition();
            }
            else
            {
                usingHostTempo = false;
            }
        }
        else
        {
            usingHostTempo = false;
        }

        if (!usingHostTempo)
        {
            bpm = 70.0;
            if (isPlaying)
            {
                timeInSamples += numSamples;
                ppqPosition = (timeInSamples / sampleRate) * (bpm / 60.0);
            }
        }
    }

    double getBPM() const { return bpm; }
    double getPPQ() const { return ppqPosition; }
    bool getIsPlaying() const { return isPlaying; }
    double getSampleRate() const { return currentSampleRate; }

    int barFractionToSamples(double barFraction) const
    {
        double beatsPerBar = 4.0;
        double secondsPerBeat = 60.0 / bpm;
        double secondsPerBar = secondsPerBeat * beatsPerBar;
        return static_cast<int>(barFraction * secondsPerBar * currentSampleRate);
    }

    double getCurrentBar() const
    {
        return ppqPosition / 4.0;
    }

private:
    double bpm = 70.0;
    double ppqPosition = 0.0;
    int64_t timeInSamples = 0;
    double currentSampleRate = 44100.0;
    bool usingHostTempo = false;
    bool isPlaying = false;
};
