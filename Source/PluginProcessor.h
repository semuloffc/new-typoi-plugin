#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "Parameters/ParameterLayout.h"
#include "Engine/HostClock.h"
#include "Engine/ScaleQuantizer.h"
#include "Engine/DewEngine.h"
#include "Engine/BloomEngine.h"
#include "Engine/BreezeEngine.h"
#include "Engine/RootsEngine.h"
#include "Engine/BurstEngine.h"
#include "Engine/Greenhouse.h"
#include "Engine/MistFilter.h"
#include "Engine/PocketDucker.h"
#include "Engine/GrowthMapper.h"

class PetrichorAudioProcessor : public juce::AudioProcessor
{
public:
    PetrichorAudioProcessor();
    ~PetrichorAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    juce::ValueTree patternState{"DewPattern"};
    juce::ValueTree chordState{"BloomChords"};

private:
    HostClock hostClock;
    ScaleQuantizer quantizer;

    DewEngine dewEngine;
    BloomEngine bloomEngine;
    BreezeEngine breezeEngine;
    RootsEngine rootsEngine;
    BurstEngine burstEngine;

    Greenhouse greenhouse;
    MistFilter mistFilter;
    PocketDucker pocketDucker;

    juce::AudioBuffer<float> dewBuffer;
    juce::AudioBuffer<float> bloomBuffer;
    juce::AudioBuffer<float> breezeBuffer;
    juce::AudioBuffer<float> rootsBuffer;
    juce::AudioBuffer<float> burstBuffer;
    juce::AudioBuffer<float> reverbBuffer;

    juce::SmoothedValue<float> dewLevelSmooth;
    juce::SmoothedValue<float> bloomLevelSmooth;
    juce::SmoothedValue<float> breezeLevelSmooth;
    juce::SmoothedValue<float> rootsLevelSmooth;
    juce::SmoothedValue<float> burstLevelSmooth;
    juce::SmoothedValue<float> masterGainSmooth;

    int burstBarCounter = 0;
    double lastBarPosition = 0.0;
    bool lastGrowthWasAbove66 = false;

    void initializePatternState();
    void initializeChordState();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PetrichorAudioProcessor)
};
