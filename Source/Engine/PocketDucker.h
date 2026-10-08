#pragma once
#include <juce_dsp/juce_dsp.h>

class PocketDucker
{
public:
    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;

        bpFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 2300.0f, 1.5f);
        bpFilter.reset();

        peakFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 2300.0f, 1.2f, 1.0f);
        peakFilter.reset();

        envelope.reset(sampleRate, 0.02);
        envelope.setCurrentAndTargetValue(0.0f);
    }

    void processSidechain(const juce::AudioBuffer<float>& sidechain)
    {
        if (sidechain.getNumChannels() == 0 || sidechain.getNumSamples() == 0)
            return;

        for (int sample = 0; sample < sidechain.getNumSamples(); ++sample)
        {
            float input = sidechain.getSample(0, sample);
            float filtered = bpFilter.processSample(input);
            float energy = filtered * filtered;

            if (energy > currentPeak)
            {
                currentPeak = energy;
                envelope.reset(currentSampleRate, 0.005);
                envelope.setTargetValue(std::sqrt(energy));
            }
            else
            {
                currentPeak *= 0.999f;
                envelope.reset(currentSampleRate, 0.15);
                envelope.setTargetValue(std::sqrt(currentPeak));
            }
        }
    }

    void applyDucking(juce::AudioBuffer<float>& buffer, float amount)
    {
        if (amount < 0.01f)
            return;

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float envValue = envelope.getNextValue();
            float reduction = juce::jlimit(0.0f, 1.0f, envValue * amount / 100.0f);

            float gainReduction = 1.0f - reduction;

            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                float input = buffer.getSample(ch, sample);
                float output = peakFilter.processSample(input) * gainReduction;
                buffer.setSample(ch, sample, output);
            }
        }
    }

private:
    juce::dsp::IIR::Filter<float> bpFilter;
    juce::dsp::IIR::Filter<float> peakFilter;
    juce::SmoothedValue<float> envelope;

    double currentSampleRate = 44100.0;
    float currentPeak = 0.0f;
};
