#pragma once
#include <juce_dsp/juce_dsp.h>

class MistFilter
{
public:
    void prepare(double sampleRate)
    {
        for (auto& filter : filters)
        {
            filter.prepare({sampleRate, 512, 2});
            filter.reset();
        }
    }

    void setCutoff(double sampleRate, float cutoffHz)
    {
        auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoffHz);

        for (auto& filter : filters)
            filter.coefficients = coeffs;
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        juce::dsp::AudioBlock<float> block(buffer);

        for (auto& filter : filters)
        {
            juce::dsp::ProcessContextReplacing<float> context(block);
            filter.process(context);
        }
    }

private:
    std::array<juce::dsp::IIR::Filter<float>, 4> filters;
};
