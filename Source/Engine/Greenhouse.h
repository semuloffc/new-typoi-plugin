#pragma once
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <array>

class Greenhouse
{
public:
    void prepare(double sampleRate, int maxBlockSize)
    {
        currentSampleRate = sampleRate;

        for (auto& line : delayLines)
            line.setMaximumDelayInSamples(static_cast<int>(6.0 * sampleRate));

        predelayLine.setMaximumDelayInSamples(static_cast<int>(0.1 * sampleRate));

        highpass.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 120.0f);
        damping.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 5000.0f);

        highpass.reset();
        damping.reset();

        for (auto& line : delayLines)
            line.reset();
        predelayLine.reset();

        lfoPhase[0] = 0.0f;
        lfoPhase[1] = 0.0f;
    }

    void setParameters(float size, float dampFreq, float lowcutFreq, float predelayMs)
    {
        const int primes[] = {2111, 2333, 2557, 2777, 3001, 3221, 3449, 3671};

        for (size_t i = 0; i < 8; ++i)
        {
            float baseDelay = primes[i] / currentSampleRate;
            delayLengths[i] = baseDelay * size / 2.5f;
        }

        damping.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, dampFreq);
        highpass.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, lowcutFreq);

        predelayTime = predelayMs * 0.001f;
    }

    void process(const juce::AudioBuffer<float>& input, juce::AudioBuffer<float>& output, float mix)
    {
        int numSamples = input.getNumSamples();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float inL = input.getSample(0, sample);
            float inR = input.getSample(1, sample);
            float inMono = (inL + inR) * 0.5f;

            inMono = highpass.processSample(inMono);

            predelayLine.pushSample(0, inMono);
            float delayed = predelayLine.popSample(0, predelayTime * currentSampleRate);

            std::array<float, 8> lineOuts;
            for (size_t i = 0; i < 8; ++i)
            {
                delayLines[i].pushSample(0, state[i]);

                float modulationDepth = (i == 2 || i == 5) ? 0.0003f : 0.0f;
                float lfoIndex = (i == 2) ? 0 : 1;
                float modulation = std::sin(lfoPhase[lfoIndex]) * modulationDepth * currentSampleRate;

                lineOuts[i] = delayLines[i].popSample(0, delayLengths[i] * currentSampleRate + modulation);
            }

            std::array<float, 8> hadamard;
            hadamard[0] = lineOuts[0] + lineOuts[1] + lineOuts[2] + lineOuts[3] + lineOuts[4] + lineOuts[5] + lineOuts[6] + lineOuts[7];
            hadamard[1] = lineOuts[0] - lineOuts[1] + lineOuts[2] - lineOuts[3] + lineOuts[4] - lineOuts[5] + lineOuts[6] - lineOuts[7];
            hadamard[2] = lineOuts[0] + lineOuts[1] - lineOuts[2] - lineOuts[3] + lineOuts[4] + lineOuts[5] - lineOuts[6] - lineOuts[7];
            hadamard[3] = lineOuts[0] - lineOuts[1] - lineOuts[2] + lineOuts[3] + lineOuts[4] - lineOuts[5] - lineOuts[6] + lineOuts[7];
            hadamard[4] = lineOuts[0] + lineOuts[1] + lineOuts[2] + lineOuts[3] - lineOuts[4] - lineOuts[5] - lineOuts[6] - lineOuts[7];
            hadamard[5] = lineOuts[0] - lineOuts[1] + lineOuts[2] - lineOuts[3] - lineOuts[4] + lineOuts[5] - lineOuts[6] + lineOuts[7];
            hadamard[6] = lineOuts[0] + lineOuts[1] - lineOuts[2] - lineOuts[3] - lineOuts[4] - lineOuts[5] + lineOuts[6] + lineOuts[7];
            hadamard[7] = lineOuts[0] - lineOuts[1] - lineOuts[2] + lineOuts[3] - lineOuts[4] + lineOuts[5] + lineOuts[6] - lineOuts[7];

            for (size_t i = 0; i < 8; ++i)
            {
                hadamard[i] *= 0.353553f;
                state[i] = delayed * 0.3f + damping.processSample(hadamard[i]) * 0.85f;
            }

            lfoPhase[0] += 0.15f * juce::MathConstants<float>::twoPi / currentSampleRate;
            lfoPhase[1] += 0.15f * juce::MathConstants<float>::twoPi / currentSampleRate;

            if (lfoPhase[0] > juce::MathConstants<float>::twoPi) lfoPhase[0] -= juce::MathConstants<float>::twoPi;
            if (lfoPhase[1] > juce::MathConstants<float>::twoPi) lfoPhase[1] -= juce::MathConstants<float>::twoPi;

            float reverbL = (state[0] + state[2] + state[4] + state[6]) * 0.25f;
            float reverbR = (state[1] + state[3] + state[5] + state[7]) * 0.25f;

            float wetGain = mix / 100.0f;
            float dryGain = 1.0f - wetGain;

            output.addSample(0, sample, inL * dryGain + reverbL * wetGain);
            output.addSample(1, sample, inR * dryGain + reverbR * wetGain);
        }
    }

private:
    std::array<juce::dsp::DelayLine<float>, 8> delayLines;
    juce::dsp::DelayLine<float> predelayLine{8820};

    juce::dsp::IIR::Filter<float> highpass;
    juce::dsp::IIR::Filter<float> damping;

    std::array<float, 8> state{};
    std::array<float, 8> delayLengths{};
    std::array<float, 2> lfoPhase{};

    double currentSampleRate = 44100.0;
    float predelayTime = 0.01f;
};
