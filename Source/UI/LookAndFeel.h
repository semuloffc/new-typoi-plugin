#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "Theme.h"

class PetrichorLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PetrichorLookAndFeel()
    {
        setColour(juce::Slider::rotarySliderFillColourId, Theme::Engine::BLOOM::fill);
        setColour(juce::Slider::rotarySliderOutlineColourId, Theme::Background::divider);
        setColour(juce::Slider::thumbColourId, Theme::Engine::BLOOM::fill);

        juce::Font::findFonts(customFonts);
    }

    juce::Font getCustomFont(float height, bool medium = false)
    {
        for (auto& font : customFonts)
        {
            if (font.getTypefaceName().contains("Inter"))
            {
                if (medium && font.getTypefaceStyle().contains("Medium"))
                    return juce::Font(font).withHeight(height);
                else if (!medium && font.getTypefaceStyle().contains("Regular"))
                    return juce::Font(font).withHeight(height);
            }
        }

        return juce::Font(height);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        g.setColour(Theme::Background::divider);
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.strokePath(backgroundArc, juce::PathStrokeType(3.0f));

        g.setColour(Theme::Engine::BLOOM::stroke);
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        g.strokePath(valueArc, juce::PathStrokeType(4.0f));

        auto pointerRadius = 7.0f;
        auto pointerX = centreX + radius * std::cos(angle - juce::MathConstants<float>::halfPi);
        auto pointerY = centreY + radius * std::sin(angle - juce::MathConstants<float>::halfPi);

        g.setColour(Theme::Background::top);
        g.fillEllipse(pointerX - pointerRadius - 1.5f, pointerY - pointerRadius - 1.5f,
                     (pointerRadius + 1.5f) * 2.0f, (pointerRadius + 1.5f) * 2.0f);

        g.setColour(Theme::Engine::BLOOM::fill);
        g.fillEllipse(pointerX - pointerRadius, pointerY - pointerRadius, pointerRadius * 2.0f, pointerRadius * 2.0f);
    }

private:
    juce::Array<juce::Font> customFonts;
};
