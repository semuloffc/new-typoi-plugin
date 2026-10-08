#pragma once
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include "Noise.h"

class RootsEngine
{
public:
    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;

        hpFilterL.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 40.0f);
        hpFilterR.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 40.0f);
        lpFilterL.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 200.0f);
        lpFilterR.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 200.0f);

        hpFilterL.reset();
        hpFilterR.reset();
        lpFilterL.reset();
        lpFilterR.reset();

        breathePhase = 0.0f;
    }

    void process(juce::AudioBuffer<float>& buffer, float rate, float depth, float monoFreq)
    {
        int numSamples = buffer.getNumSamples();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float whiteL = noise.nextWhite();
            float whiteR = noiseR.nextWhite();

            float brownL = noise.processBrown(whiteL);
            float brownR = noiseR.processBrown(whiteR);

            brownL = hpFilterL.processSample(brownL);
            brownL = lpFilterL.processSample(brownL);

            brownR = hpFilterR.processSample(brownR);
            brownR = lpFilterR.processSample(brownR);

            breathePhase += rate * juce::MathConstants<float>::twoPi / currentSampleRate;
            if (breathePhase > juce::MathConstants<float>::twoPi)
                breathePhase -= juce::MathConstants<float>::twoPi;

            float breatheEnv = 0.5f + 0.5f * std::sin(breathePhase);
            float breatheMod = 1.0f - (depth / 100.0f) * (1.0f - breatheEnv);

            brownL *= breatheMod * 0.3f;
            brownR *= breatheMod * 0.3f;

            float outL = brownL;
            float outR = brownR;

            if (monoFreq > 1.0f)
            {
                float mono = (brownL + brownR) * 0.5f;
                outL = mono;
                outR = mono;
            }

            buffer.addSample(0, sample, outL);
            buffer.addSample(1, sample, outR);
        }
    }

private:
    Noise noise{0x524F5400};
    Noise noiseR{0x524F5401};

    juce::dsp::IIR::Filter<float> hpFilterL, hpFilterR;
    juce::dsp::IIR::Filter<float> lpFilterL, lpFilterR;

    double currentSampleRate = 44100.0;
    float breathePhase = 0.0f;
};
