#include "ParameterLayout.h"
#include "ParameterIDs.h"

namespace ParameterLayout
{
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID{ParameterIDs::root, 1},
            "Root",
            juce::StringArray{"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"},
            0));

        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID{ParameterIDs::scale, 1},
            "Scale",
            juce::StringArray{"Neutral-6", "Major-6", "Minor Pent", "Major Pent", "Chromatic"},
            0));

        layout.add(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{ParameterIDs::followMidi, 1},
            "Follow MIDI",
            false));

        layout.add(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{ParameterIDs::dewEnable, 1},
            "DEW Enable",
            true));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::dewLevel, 1},
            "DEW Level",
            juce::NormalisableRange<float>{-30.0f, 0.0f, 0.1f},
            -24.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " dB"; }));

        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID{ParameterIDs::dewOctave, 1},
            "DEW Octave",
            juce::StringArray{"5", "6", "7", "8"},
            2));

        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID{ParameterIDs::dewRate, 1},
            "DEW Rate",
            juce::StringArray{"1/4", "1/8", "1/16", "1/8T"},
            2));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::dewGate, 1},
            "DEW Gate",
            juce::NormalisableRange<float>{10.0f, 400.0f, 1.0f},
            190.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(static_cast<int>(value)) + " ms"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::dewRelease, 1},
            "DEW Release",
            juce::NormalisableRange<float>{20.0f, 600.0f, 1.0f},
            120.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(static_cast<int>(value)) + " ms"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::dewPurity, 1},
            "DEW Purity",
            juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
            100.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " %"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::dewMaterial, 1},
            "DEW Material",
            juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
            0.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " %"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::dewProb, 1},
            "DEW Probability",
            juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
            100.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " %"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::dewHumanize, 1},
            "DEW Humanize",
            juce::NormalisableRange<float>{0.0f, 30.0f, 0.1f},
            4.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " ms"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::dewSpread, 1},
            "DEW Spread",
            juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
            0.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " %"; }));

        layout.add(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{ParameterIDs::bloomEnable, 1},
            "BLOOM Enable",
            true));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::bloomLevel, 1},
            "BLOOM Level",
            juce::NormalisableRange<float>{-30.0f, 0.0f, 0.1f},
            -26.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " dB"; }));

        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID{ParameterIDs::bloomBars, 1},
            "BLOOM Bars",
            juce::StringArray{"1", "2", "4"},
            0));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::bloomAttack, 1},
            "BLOOM Attack",
            juce::NormalisableRange<float>{0.2f, 8.0f, 0.01f, 0.4f},
            1.5f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 2) + " s"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::bloomBright, 1},
            "BLOOM Brightness",
            juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
            40.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " %"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::bloomWidth, 1},
            "BLOOM Width",
            juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
            85.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " %"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::bloomDrift, 1},
            "BLOOM Drift",
            juce::NormalisableRange<float>{0.0f, 20.0f, 0.1f},
            4.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " ct"; }));

        layout.add(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{ParameterIDs::breezeEnable, 1},
            "BREEZE Enable",
            true));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::breezeLevel, 1},
            "BREEZE Level",
            juce::NormalisableRange<float>{-40.0f, 0.0f, 0.1f},
            -34.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " dB"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::breezeCenter, 1},
            "BREEZE Center",
            juce::NormalisableRange<float>{2000.0f, 8000.0f, 1.0f, 0.4f},
            4600.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value / 1000.0f, 1) + " kHz"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::breezeWidth, 1},
            "BREEZE Width",
            juce::NormalisableRange<float>{0.5f, 3.0f, 0.01f},
            0.8f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 2) + " oct"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::breezeSwell, 1},
            "BREEZE Swell",
            juce::NormalisableRange<float>{1.0f, 8.0f, 1.0f},
            2.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(static_cast<int>(value)) + " bars"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::breezeDepth, 1},
            "BREEZE Depth",
            juce::NormalisableRange<float>{0.0f, 24.0f, 0.1f},
            18.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " dB"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::breezeCut, 1},
            "BREEZE Cut",
            juce::NormalisableRange<float>{0.0f, 20.0f, 0.1f},
            11.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " dB"; }));

        layout.add(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{ParameterIDs::rootsEnable, 1},
            "ROOTS Enable",
            true));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::rootsLevel, 1},
            "ROOTS Level",
            juce::NormalisableRange<float>{-40.0f, 0.0f, 0.1f},
            -30.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " dB"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::rootsRate, 1},
            "ROOTS Rate",
            juce::NormalisableRange<float>{0.1f, 1.0f, 0.01f},
            0.3f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 2) + " Hz"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::rootsDepth, 1},
            "ROOTS Depth",
            juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
            60.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " %"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::rootsMono, 1},
            "ROOTS Mono",
            juce::NormalisableRange<float>{0.0f, 250.0f, 1.0f},
            120.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) {
                if (value < 1.0f) return juce::String("Off");
                return juce::String(static_cast<int>(value)) + " Hz";
            }));

        layout.add(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{ParameterIDs::burstEnable, 1},
            "BURST Enable",
            true));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::burstLevel, 1},
            "BURST Level",
            juce::NormalisableRange<float>{-24.0f, 0.0f, 0.1f},
            -9.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " dB"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::burstSize, 1},
            "BURST Size",
            juce::NormalisableRange<float>{0.2f, 2.5f, 0.01f},
            0.9f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 2) + " s"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::burstTone, 1},
            "BURST Tone",
            juce::NormalisableRange<float>{-100.0f, 100.0f, 1.0f},
            0.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(static_cast<int>(value)); }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::burstEvery, 1},
            "BURST Every",
            juce::NormalisableRange<float>{1.0f, 8.0f, 1.0f},
            4.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(static_cast<int>(value)) + " bars"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::burstProb, 1},
            "BURST Probability",
            juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
            25.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " %"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::mistCutoff, 1},
            "Mist Cutoff",
            juce::NormalisableRange<float>{1500.0f, 16000.0f, 1.0f, 0.3f},
            6000.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value / 1000.0f, 1) + " kHz"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::ghMix, 1},
            "Greenhouse Mix",
            juce::NormalisableRange<float>{0.0f, 60.0f, 0.1f},
            25.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " %"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::ghSize, 1},
            "Greenhouse Size",
            juce::NormalisableRange<float>{0.5f, 6.0f, 0.01f, 0.4f},
            2.5f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 2) + " s"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::ghDamp, 1},
            "Greenhouse Damping",
            juce::NormalisableRange<float>{2000.0f, 12000.0f, 1.0f, 0.4f},
            5000.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value / 1000.0f, 1) + " kHz"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::ghPredelay, 1},
            "Greenhouse Predelay",
            juce::NormalisableRange<float>{0.0f, 80.0f, 1.0f},
            10.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(static_cast<int>(value)) + " ms"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::ghLowcut, 1},
            "Greenhouse Lowcut",
            juce::NormalisableRange<float>{20.0f, 400.0f, 1.0f, 0.4f},
            120.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(static_cast<int>(value)) + " Hz"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::pocketAmount, 1},
            "Pocket Amount",
            juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
            0.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " %"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::masterGain, 1},
            "Master Gain",
            juce::NormalisableRange<float>{-24.0f, 6.0f, 0.1f},
            0.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " dB"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::growth, 1},
            "Growth",
            juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
            55.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " %"; }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::weatherStorm, 1},
            "Weather Storm",
            juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
            30.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1); }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ParameterIDs::weatherHumid, 1},
            "Weather Humid",
            juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
            40.0f,
            juce::String{},
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1); }));

        return layout;
    }
}
