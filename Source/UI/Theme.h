#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace Theme
{
    namespace Background
    {
        inline const juce::Colour top{0xFFF3F8F6};
        inline const juce::Colour mid{0xFFE8F0EE};
        inline const juce::Colour bottom{0xFFB3C7C2};
        inline const juce::Colour well{0xFFDCE8E5};
        inline const juce::Colour divider{0xFFC9D9D5};
    }

    namespace Text
    {
        inline const juce::Colour primary{0xFF2B3A38};
        inline const juce::Colour secondary{0xFF5A6B68};
        inline const juce::Colour muted{0xFF7F918D};
        inline const juce::Colour inverse{0xFFF3F8F6};
    }

    namespace Engine
    {
        namespace DEW
        {
            inline const juce::Colour tint{0xFFE3F3F8};
            inline const juce::Colour fill{0xFFBFE3EE};
            inline const juce::Colour stroke{0xFF7FBBD0};
        }

        namespace BLOOM
        {
            inline const juce::Colour tint{0xFFDCEDE6};
            inline const juce::Colour fill{0xFFA6CFBF};
            inline const juce::Colour stroke{0xFF6FA793};
        }

        namespace BREEZE
        {
            inline const juce::Colour tint{0xFFE4EAEC};
            inline const juce::Colour fill{0xFFBCC9CF};
            inline const juce::Colour stroke{0xFF8FA3AB};
        }

        namespace ROOTS
        {
            inline const juce::Colour tint{0xFFDDE5DF};
            inline const juce::Colour fill{0xFF5F7566};
            inline const juce::Colour stroke{0xFF3F5246};
        }

        namespace BURST
        {
            inline const juce::Colour tint{0xFFF8E4E7};
            inline const juce::Colour fill{0xFFE9A8B0};
            inline const juce::Colour stroke{0xFFD98794};
        }
    }

    inline juce::Colour getHoverColour(const juce::Colour& baseColour)
    {
        return baseColour.brighter(0.1f);
    }

    inline juce::Colour getPressedColour(const juce::Colour& baseColour)
    {
        return baseColour.darker(0.1f);
    }
}
