#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "Theme.h"

class GlassBackground : public juce::Component, private juce::Timer
{
public:
    GlassBackground()
    {
        blobs.resize(5);
        for (size_t i = 0; i < blobs.size(); ++i)
        {
            blobs[i].x = juce::Random::getSystemRandom().nextFloat() * 1280.0f;
            blobs[i].y = juce::Random::getSystemRandom().nextFloat() * 720.0f;
            blobs[i].radius = 140.0f + juce::Random::getSystemRandom().nextFloat() * 80.0f;
            blobs[i].vx = (juce::Random::getSystemRandom().nextFloat() - 0.5f) * 0.02f;
            blobs[i].vy = (juce::Random::getSystemRandom().nextFloat() - 0.5f) * 0.02f;
            blobs[i].colour = getBlobColour(i);
        }

        startTimerHz(60);
    }

    void paint(juce::Graphics& g) override
    {
        juce::ColourGradient gradient(Theme::Background::top, 0, 0,
                                     Theme::Background::bottom, 0, static_cast<float>(getHeight()), false);
        gradient.addColour(0.5, Theme::Background::mid);
        g.setGradientFill(gradient);
        g.fillAll();

        for (const auto& blob : blobs)
        {
            g.setColour(blob.colour.withAlpha(0.35f));
            g.fillEllipse(blob.x - blob.radius, blob.y - blob.radius, blob.radius * 2.0f, blob.radius * 2.0f);
        }
    }

    void resized() override
    {
    }

private:
    struct Blob
    {
        float x, y, radius;
        float vx, vy;
        juce::Colour colour;
    };

    std::vector<Blob> blobs;

    void timerCallback() override
    {
        for (auto& blob : blobs)
        {
            blob.x += blob.vx;
            blob.y += blob.vy;

            if (blob.x < 0 || blob.x > 1280.0f) blob.vx = -blob.vx;
            if (blob.y < 0 || blob.y > 720.0f) blob.vy = -blob.vy;
        }
        repaint();
    }

    juce::Colour getBlobColour(size_t index)
    {
        switch (index % 4)
        {
            case 0: return Theme::Engine::BLOOM::fill;
            case 1: return Theme::Engine::DEW::fill;
            case 2: return Theme::Engine::BURST::tint;
            case 3: return Theme::Engine::BREEZE::fill;
            default: return Theme::Engine::BLOOM::fill;
        }
    }
};
