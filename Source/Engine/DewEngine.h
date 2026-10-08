#pragma once
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>
#include "Noise.h"
#include "ModalBank.h"
#include "ScaleQuantizer.h"
#include "HostClock.h"

class DewEngine
{
public:
    DewEngine()
    {
        for (int i = 0; i < 2048; ++i)
            sineTable[i] = std::sin(i * 2.0f * juce::MathConstants<float>::pi / 2048.0f);

        pattern = {1,2,1,2,0,1,2,1,2,1,2,0,1,2,1,2};
    }

    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;

        for (auto& voice : voices)
        {
            voice.modalBank.prepare(sampleRate);
            voice.attackRamp.reset(sampleRate, 0.002);
            voice.gateEnv.reset(sampleRate, 0.02);
            voice.releaseEnv.reset(sampleRate, 0.02);
        }
    }

    void setPattern(const std::array<int, 16>& newPattern)
    {
        pattern = newPattern;
    }

    void process(juce::AudioBuffer<float>& buffer, const HostClock& clock, const ScaleQuantizer& quantizer,
                 int octave, float rate, float gate, float release, float purity, float material,
                 float prob, float humanize, float spread)
    {
        int numSamples = buffer.getNumSamples();

        int samplesPerStep = calculateStepLength(clock, rate);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            if (stepCounter >= samplesPerStep)
            {
                stepCounter = 0;
                advanceStep(quantizer, octave, purity, material, prob, humanize);
            }

            float outL = 0.0f;
            float outR = 0.0f;

            for (auto& voice : voices)
            {
                if (voice.isActive)
                {
                    float sample = processSine(voice.phase, voice.increment);

                    if (purity < 100.0f)
                    {
                        float modalSample = voice.modalBank.process();
                        float modalMix = (100.0f - purity) / 100.0f;
                        sample = sample * (1.0f - modalMix * 0.5f) + modalSample * modalMix;
                    }

                    float env = voice.attackRamp.getNextValue();

                    if (voice.gateRemaining > 0)
                    {
                        voice.gateRemaining--;
                        env *= voice.gateEnv.getNextValue();
                    }
                    else
                    {
                        env *= voice.releaseEnv.getNextValue();
                        if (voice.releaseEnv.getTargetValue() == 0.0f && voice.releaseEnv.isSmoothing() == false)
                            voice.isActive = false;
                    }

                    sample *= env;

                    float pan = voice.pan;
                    outL += sample * (1.0f - pan);
                    outR += sample * pan;

                    voice.phase += voice.increment;
                    if (voice.phase >= 2048.0f)
                        voice.phase -= 2048.0f;
                }
            }

            float mono = (outL + outR) * 0.5f;
            buffer.setSample(0, sample, buffer.getSample(0, sample) + mono);
            buffer.setSample(1, sample, buffer.getSample(1, sample) + mono);

            stepCounter++;
        }

        gateLength = static_cast<int>(gate * 0.001f * currentSampleRate);
        releaseLength = static_cast<int>(release * 0.001f * currentSampleRate);
    }

private:
    struct Voice
    {
        bool isActive = false;
        float phase = 0.0f;
        float increment = 0.0f;
        int gateRemaining = 0;
        float pan = 0.5f;
        ModalBank modalBank;
        juce::SmoothedValue<float> attackRamp;
        juce::SmoothedValue<float> gateEnv;
        juce::SmoothedValue<float> releaseEnv;
    };

    std::array<float, 2048> sineTable;
    std::array<Voice, 8> voices;
    std::array<int, 16> pattern;

    Noise noise{0x44455700};
    int currentStep = 0;
    int stepCounter = 0;
    double currentSampleRate = 44100.0;
    int gateLength = 0;
    int releaseLength = 0;

    float processSine(float phase, float increment)
    {
        int index = static_cast<int>(phase);
        float frac = phase - index;

        float s0 = sineTable[index];
        float s1 = sineTable[(index + 1) % 2048];

        return s0 + frac * (s1 - s0);
    }

    int calculateStepLength(const HostClock& clock, float rate)
    {
        double barFraction = 0.0625;

        if (rate < 1.5f)
            barFraction = 0.25;
        else if (rate < 2.5f)
            barFraction = 0.125;
        else if (rate < 3.5f)
            barFraction = 0.0625;
        else
            barFraction = 0.08333;

        return clock.barFractionToSamples(barFraction);
    }

    void advanceStep(const ScaleQuantizer& quantizer, int octave, float purity, float material, float prob, float humanize)
    {
        currentStep = (currentStep + 1) % 16;

        int patternValue = pattern[currentStep];

        if (patternValue > 0 && (noise.nextWhite() * 0.5f + 0.5f) * 100.0f < prob)
        {
            int baseNote = 60 + (octave + 5) * 12;
            int scaleIndex = (patternValue == 1) ? 0 : 2;

            int midiNote = quantizer.quantizeToScale(baseNote + scaleIndex);
            float frequency = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);

            Voice* freeVoice = nullptr;
            for (auto& voice : voices)
            {
                if (!voice.isActive)
                {
                    freeVoice = &voice;
                    break;
                }
            }

            if (freeVoice != nullptr)
            {
                freeVoice->isActive = true;
                freeVoice->phase = 0.0f;
                freeVoice->increment = frequency * 2048.0f / currentSampleRate;
                freeVoice->gateRemaining = gateLength;
                freeVoice->pan = 0.5f;

                freeVoice->attackRamp.setCurrentAndTargetValue(0.0f);
                freeVoice->attackRamp.setTargetValue(0.3f);

                freeVoice->gateEnv.setCurrentAndTargetValue(1.0f);
                freeVoice->releaseEnv.setCurrentAndTargetValue(1.0f);
                freeVoice->releaseEnv.reset(currentSampleRate, releaseLength / currentSampleRate);
                freeVoice->releaseEnv.setTargetValue(0.0f);

                if (purity < 100.0f)
                {
                    freeVoice->modalBank.reset();
                    freeVoice->modalBank.trigger(frequency, material);
                }
            }
        }
    }
};
