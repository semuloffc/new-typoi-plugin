#pragma once
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include "Noise.h"
#include "HostClock.h"

class BreezeEngine
{
public:
    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;

        for (auto& stage : bpfStages)
        {
            stage.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 4600.0f, 0.8f);
            stage.reset();
        }

        gustEnvelope.reset(sampleRate, 0.02);
        gustEnvelope.setCurrentAndTargetValue(0.0f);
    }

    void process(juce::AudioBuffer<float>& buffer, const HostClock& clock,
                 float center, float width, float swell, float depth, float cut)
    {
        int numSamples = buffer.getNumSamples();

        updateFilters(center, width);

        double currentBar = clock.getCurrentBar();
        double barInCycle = std::fmod(currentBar, swell);
        double cyclePhase = barInCycle / swell;

        if (cyclePhase < lastCyclePhase)
        {
            gustPeak = depth;
            gustFloor = 0.0f;
        }

        lastCyclePhase = cyclePhase;

        float targetDb = gustFloor + (gustPeak - gustFloor) * static_cast<float>(cyclePhase);

        if (cyclePhase > 0.99)
        {
            gustFloor = targetDb - cut;
            if (gustFloor < 0.0f) gustFloor = 0.0f;
        }

        float targetLinear = juce::Decibels::decibelsToGain(targetDb);
        gustEnvelope.setTargetValue(targetLinear);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float whiteL = noise.nextWhite();
            float whiteR = noise.nextWhite();

            float filteredL = whiteL;
            float filteredR = whiteR;

            for (auto& stage : bpfStages)
            {
                filteredL = stage.processSample(filteredL);
            }

            for (auto& stage : bpfStages)
            {
                filteredR = stage.processSample(filteredR);
            }

            float env = gustEnvelope.getNextValue();

            filteredL *= env * 0.5f;
            filteredR *= env * 0.5f;

            float mono = (filteredL + filteredR) * 0.5f;
            float side = (filteredL - filteredR) * 0.2f;

            buffer.addSample(0, sample, mono + side);
            buffer.addSample(1, sample, mono - side);
        }
    }

private:
    Noise noise{0x42525A00};
    std::array<juce::dsp::IIR::Filter<float>, 4> bpfStages;
    juce::SmoothedValue<float> gustEnvelope;

    double currentSampleRate = 44100.0;
    double lastCyclePhase = 0.0;
    float gustPeak = 18.0f;
    float gustFloor = 0.0f;

    void updateFilters(float center, float widthOct)
    {
        float q = 1.0f / widthOct;

        for (auto& stage : bpfStages)
        {
            stage.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(
                currentSampleRate, center, q);
        }
    }
};
