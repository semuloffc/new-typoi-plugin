#pragma once
#include <juce_dsp/juce_dsp.h>

class GrowthMapper
{
public:
    struct Gains
    {
        float dew = 1.0f;
        int bloomVoices = 6;
        float bloom = 1.0f;
        float breeze = 1.0f;
        float roots = 1.0f;
        bool burstArmed = true;
    };

    static Gains calculate(float growth, float weatherStorm, float weatherHumid)
    {
        Gains gains;

        if (growth < 33.0f)
        {
            gains.dew = 1.0f;
            gains.bloomVoices = 1;
            gains.bloom = juce::Decibels::decibelsToGain(-24.0f);
            gains.breeze = 0.5f;
            gains.roots = 0.0f;
            gains.burstArmed = false;
        }
        else if (growth < 66.0f)
        {
            float t = (growth - 33.0f) / 33.0f;
            gains.dew = 1.0f;
            gains.bloomVoices = 1 + static_cast<int>(t * 5.0f);
            gains.bloom = juce::Decibels::decibelsToGain(-24.0f + t * 24.0f);
            gains.breeze = 0.5f + t * 0.5f;
            gains.roots = 0.0f;
            gains.burstArmed = false;
        }
        else
        {
            float t = (growth - 66.0f) / 34.0f;
            gains.dew = 1.0f;
            gains.bloomVoices = 6;
            gains.bloom = 1.0f;
            gains.breeze = 1.0f;
            gains.roots = t;
            gains.burstArmed = true;
        }

        if (weatherStorm > 0.0f)
        {
            gains.breeze *= (1.0f + weatherStorm * 0.005f);
        }

        return gains;
    }

    static float applyWeatherToGreenhouse(float baseValue, float paramId, float weatherHumid)
    {
        return baseValue;
    }
};
