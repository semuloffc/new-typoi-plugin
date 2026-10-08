#pragma once
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include "Noise.h"

class BurstEngine
{
public:
    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;

        hpFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 150.0f);
        hpFilter.reset();

        thumpPhase = 0.0f;
        noiseEnvelope.reset(sampleRate, 0.02);
        thumpEnvelope.reset(sampleRate, 0.02);
    }

    void trigger()
    {
        isTriggered = true;
        thumpPhase = 0.0f;
        noiseEnvelope.setCurrentAndTargetValue(1.0f);
        thumpEnvelope.setCurrentAndTargetValue(1.0f);
    }

    void process(juce::AudioBuffer<float>& buffer, float size, float tone)
    {
        int numSamples = buffer.getNumSamples();

        float decayRate = std::exp(-1.0f / (size * currentSampleRate));

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float output = 0.0f;

            if (isTriggered)
            {
                float white = noise.nextWhite();
                float filtered = hpFilter.processSample(white);

                float noiseEnv = noiseEnvelope.getCurrentValue();
                noiseEnvelope.setCurrentAndTargetValue(noiseEnv * decayRate);

                float thumpEnv = thumpEnvelope.getCurrentValue();
                thumpEnvelope.setCurrentAndTargetValue(thumpEnv * 0.9992f);

                float thumpProgress = thumpPhase / (0.12f * currentSampleRate);
                float thumpFreq = 80.0f + (40.0f - 80.0f) * thumpProgress;

                thumpFreq = juce::jlimit(40.0f, 80.0f, thumpFreq);

                float thumpSample = std::sin(thumpPhase * juce::MathConstants<float>::twoPi * thumpFreq / currentSampleRate);
                thumpSample *= thumpEnv * 0.3f;

                thumpPhase += 1.0f;

                output = filtered * noiseEnv * 0.8f + thumpSample;

                if (tone != 0.0f)
                {
                    float tilt = tone / 100.0f;
                    output *= (1.0f + tilt * 0.5f);
                }

                if (noiseEnv < 0.001f && thumpEnv < 0.001f)
                    isTriggered = false;
            }

            buffer.addSample(0, sample, output);
            buffer.addSample(1, sample, output);
        }
    }

    bool isActive() const { return isTriggered; }

private:
    Noise noise{0x42525400};
    juce::dsp::IIR::Filter<float> hpFilter;

    double currentSampleRate = 44100.0;
    bool isTriggered = false;
    float thumpPhase = 0.0f;

    juce::SmoothedValue<float> noiseEnvelope;
    juce::SmoothedValue<float> thumpEnvelope;
};
