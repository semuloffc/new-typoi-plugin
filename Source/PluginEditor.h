#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "UI/LookAndFeel.h"
#include "UI/GlassBackground.h"

class PetrichorAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    PetrichorAudioProcessorEditor(PetrichorAudioProcessor&);
    ~PetrichorAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    PetrichorAudioProcessor& audioProcessor;
    juce::ComponentBoundsConstrainer constrainer;
    PetrichorLookAndFeel lookAndFeel;
    GlassBackground background;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PetrichorAudioProcessorEditor)
};
