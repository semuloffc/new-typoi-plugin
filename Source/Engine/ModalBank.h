#pragma once
#include <juce_dsp/juce_dsp.h>
#include <array>

class ModalBank
{
public:
    void prepare(double sampleRate)
    {
        for (auto& filter : filters)
            filter.prepare({sampleRate, 512, 1});
    }

    void trigger(float frequency, float material)
    {
        std::array<float, 4> ratios;

        if (material < 50.0f)
        {
            float t = material / 50.0f;
            ratios[0] = 1.0f;
            ratios[1] = 2.32f + t * (4.0f - 2.32f);
            ratios[2] = 4.25f + t * (10.0f - 4.25f);
            ratios[3] = 6.63f;
        }
        else
        {
            float t = (material - 50.0f) / 50.0f;
            ratios[0] = 1.0f;
            ratios[1] = 4.0f + t * (2.756f - 4.0f);
            ratios[2] = 10.0f + t * (5.404f - 10.0f);
            ratios[3] = 0.0f + t * 8.933f;
        }

        for (size_t i = 0; i < 4; ++i)
        {
            if (ratios[i] > 0.0f)
            {
                float modeFreq = frequency * ratios[i];
                float q = (material < 50.0f) ? 3.0f : (8.0f + material * 0.1f);

                filters[i].coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                    filters[i].coefficients->getSampleRate(),
                    modeFreq,
                    q,
                    1.0f);

                impulse[i] = 1.0f;
            }
        }
    }

    float process()
    {
        float output = 0.0f;

        for (size_t i = 0; i < 4; ++i)
        {
            if (impulse[i] > 0.00001f)
            {
                float sample = impulse[i];
                impulse[i] *= 0.9995f;

                output += filters[i].processSample(sample) * 0.25f;
            }
        }

        return output;
    }

    void reset()
    {
        for (auto& filter : filters)
            filter.reset();
        impulse.fill(0.0f);
    }

private:
    std::array<juce::dsp::IIR::Filter<float>, 4> filters;
    std::array<float, 4> impulse{};
};
