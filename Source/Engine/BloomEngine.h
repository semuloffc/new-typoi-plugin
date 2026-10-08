#pragma once
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>
#include "Noise.h"
#include "ScaleQuantizer.h"
#include "HostClock.h"

class BloomEngine
{
public:
    BloomEngine()
    {
        chordA = {0, 2, 3, 5, 7, 10};
        chordB = {3, 7, 10, 14, 17, 21};
    }

    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;

        for (auto& note : notes)
        {
            note.envelope.reset(sampleRate, 0.02);
            note.detuneAmount1 = 0.0f;
            note.detuneAmount2 = 0.0f;
            note.driftPhase = noise.nextWhite() * juce::MathConstants<float>::twoPi;
        }

        for (auto& chorus : chorusLines)
        {
            chorus.delayLine.setMaximumDelayInSamples(static_cast<int>(0.01f * sampleRate));
            chorus.lfoPhase = noise.nextWhite() * juce::MathConstants<float>::twoPi;
        }
    }

    void setChords(const std::array<int, 6>& newChordA, const std::array<int, 6>& newChordB)
    {
        chordA = newChordA;
        chordB = newChordB;
    }

    void process(juce::AudioBuffer<float>& buffer, const HostClock& clock, const ScaleQuantizer& quantizer,
                 int bars, float attack, float bright, float width, float drift)
    {
        int numSamples = buffer.getNumSamples();

        double currentBar = clock.getCurrentBar();
        int currentChord = static_cast<int>(currentBar / bars) % 2;

        if (currentChord != lastChord)
        {
            lastChord = currentChord;
            updateChord(quantizer, attack);
        }

        float slope = 2.2f - 1.6f * (bright / 100.0f);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float outL = 0.0f;
            float outR = 0.0f;

            for (size_t i = 0; i < 6; ++i)
            {
                auto& note = notes[i];

                if (note.targetFreq > 0.0f)
                {
                    float env = note.envelope.getNextValue();

                    note.driftPhase += 0.0001f;
                    if (note.driftPhase > juce::MathConstants<float>::twoPi)
                        note.driftPhase -= juce::MathConstants<float>::twoPi;

                    float driftCents = std::sin(note.driftPhase) * drift;
                    float driftMult = std::pow(2.0f, driftCents / 1200.0f);

                    for (int partial = 1; partial <= 16; ++partial)
                    {
                        float amplitude = std::pow(static_cast<float>(partial), -slope) * env;

                        float freq1 = note.targetFreq * partial * driftMult * (1.0f + note.detuneAmount1);
                        float freq2 = note.targetFreq * partial * driftMult * (1.0f + note.detuneAmount2);

                        note.phase1 += freq1 / currentSampleRate;
                        note.phase2 += freq2 / currentSampleRate;

                        if (note.phase1 >= 1.0f) note.phase1 -= 1.0f;
                        if (note.phase2 >= 1.0f) note.phase2 -= 1.0f;

                        float s1 = std::sin(note.phase1 * juce::MathConstants<float>::twoPi);
                        float s2 = std::sin(note.phase2 * juce::MathConstants<float>::twoPi);

                        float panL = 0.4f + note.pan * 0.2f;
                        float panR = 0.6f - note.pan * 0.2f;

                        outL += s1 * amplitude * panL + s2 * amplitude * (1.0f - panL);
                        outR += s1 * amplitude * panR + s2 * amplitude * (1.0f - panR);
                    }
                }
            }

            outL *= 0.01f;
            outR *= 0.01f;

            chorusLines[0].lfoPhase += 0.3f * juce::MathConstants<float>::twoPi / currentSampleRate;
            chorusLines[1].lfoPhase += 0.3f * juce::MathConstants<float>::twoPi / currentSampleRate;

            if (chorusLines[0].lfoPhase > juce::MathConstants<float>::twoPi)
                chorusLines[0].lfoPhase -= juce::MathConstants<float>::twoPi;
            if (chorusLines[1].lfoPhase > juce::MathConstants<float>::twoPi)
                chorusLines[1].lfoPhase -= juce::MathConstants<float>::twoPi;

            float delay0 = 0.003f * (1.0f + std::sin(chorusLines[0].lfoPhase) * 0.5f);
            float delay1 = 0.003f * (1.0f + std::cos(chorusLines[1].lfoPhase) * 0.5f);

            chorusLines[0].delayLine.pushSample(0, outL);
            chorusLines[1].delayLine.pushSample(0, outR);

            float chorusL = chorusLines[0].delayLine.popSample(0, delay0 * currentSampleRate);
            float chorusR = chorusLines[1].delayLine.popSample(0, delay1 * currentSampleRate);

            float mixWidth = width / 100.0f;
            float finalL = outL * 0.7f + chorusL * 0.3f * mixWidth;
            float finalR = outR * 0.7f + chorusR * 0.3f * mixWidth;

            buffer.addSample(0, sample, finalL);
            buffer.addSample(1, sample, finalR);
        }
    }

private:
    struct Note
    {
        float targetFreq = 0.0f;
        float phase1 = 0.0f;
        float phase2 = 0.0f;
        float detuneAmount1 = 0.0f;
        float detuneAmount2 = 0.0f;
        float pan = 0.5f;
        float driftPhase = 0.0f;
        juce::SmoothedValue<float> envelope;
    };

    struct ChorusLine
    {
        juce::dsp::DelayLine<float> delayLine{4410};
        float lfoPhase = 0.0f;
    };

    std::array<Note, 6> notes;
    std::array<ChorusLine, 2> chorusLines;
    std::array<int, 6> chordA;
    std::array<int, 6> chordB;

    Noise noise{0x424C4D00};
    double currentSampleRate = 44100.0;
    int lastChord = -1;

    void updateChord(const ScaleQuantizer& quantizer, float attack)
    {
        const auto& chord = (lastChord == 0) ? chordA : chordB;

        for (size_t i = 0; i < 6; ++i)
        {
            int midiNote = quantizer.quantizeToScale(60 + chord[i]);
            notes[i].targetFreq = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);

            notes[i].envelope.reset(currentSampleRate, attack);
            notes[i].envelope.setCurrentAndTargetValue(0.0f);
            notes[i].envelope.setTargetValue(0.15f);

            notes[i].detuneAmount1 = (noise.nextWhite() * 0.5f + 0.5f) * 0.002f;
            notes[i].detuneAmount2 = -(noise.nextWhite() * 0.5f + 0.5f) * 0.002f;
            notes[i].pan = (noise.nextWhite() * 0.5f + 0.5f);
        }
    }
};
