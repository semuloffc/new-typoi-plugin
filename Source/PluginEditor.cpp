#include "PluginEditor.h"

PetrichorAudioProcessorEditor::PetrichorAudioProcessorEditor(PetrichorAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&lookAndFeel);

    addAndMakeVisible(background);

    constrainer.setFixedAspectRatio(16.0 / 9.0);
    constrainer.setSizeLimits(960, 540, 1920, 1080);
    setConstrainer(&constrainer);
    setResizable(true, true);
    setSize(1280, 720);
}

PetrichorAudioProcessorEditor::~PetrichorAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void PetrichorAudioProcessorEditor::paint(juce::Graphics& g)
{
}

void PetrichorAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    float scale = bounds.getWidth() / 1280.0f;

    background.setBounds(bounds);
    background.setTransform(juce::AffineTransform::scale(scale));
}
