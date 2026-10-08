#pragma once
#include <vector>
#include <array>

class ScaleQuantizer
{
public:
    enum Scale
    {
        Neutral6 = 0,
        Major6,
        MinorPent,
        MajorPent,
        Chromatic
    };

    void setRoot(int root)
    {
        rootNote = root;
    }

    void setScale(Scale scaleType)
    {
        currentScale = scaleType;
    }

    int quantizeToScale(int midiNote) const
    {
        if (currentScale == Chromatic)
            return midiNote;

        const auto* mask = getScaleMask(currentScale);
        int octave = midiNote / 12;
        int chroma = midiNote % 12;

        int offsetChroma = (chroma - rootNote + 12) % 12;

        int lower = -1, upper = -1;
        for (int i = 0; i < 12; ++i)
        {
            if (mask[i])
            {
                if (i <= offsetChroma)
                    lower = i;
                if (i >= offsetChroma && upper == -1)
                    upper = i;
            }
        }

        if (lower == -1)
        {
            for (int i = 11; i >= 0; --i)
                if (mask[i]) { lower = i - 12; break; }
        }

        if (upper == -1)
        {
            for (int i = 0; i < 12; ++i)
                if (mask[i]) { upper = i + 12; break; }
        }

        int lowerDist = offsetChroma - lower;
        int upperDist = upper - offsetChroma;

        int quantizedOffset = (lowerDist <= upperDist) ? lower : upper;
        int quantizedChroma = (rootNote + quantizedOffset) % 12;
        if (quantizedChroma < 0) quantizedChroma += 12;

        int octaveAdjust = (rootNote + quantizedOffset) / 12;
        if (rootNote + quantizedOffset < 0) octaveAdjust -= 1;

        return octave * 12 + octaveAdjust * 12 + quantizedChroma;
    }

private:
    int rootNote = 0;
    Scale currentScale = Neutral6;

    static const bool* getScaleMask(Scale scale)
    {
        static const bool neutral6[12] = {true, false, true, true, false, true, false, true, false, false, true, false};
        static const bool major6[12] = {true, false, true, false, true, false, false, true, false, true, false, true};
        static const bool minorPent[12] = {true, false, false, true, false, true, false, true, false, false, true, false};
        static const bool majorPent[12] = {true, false, true, false, true, false, false, true, false, true, false, false};
        static const bool chromatic[12] = {true, true, true, true, true, true, true, true, true, true, true, true};

        switch (scale)
        {
            case Neutral6: return neutral6;
            case Major6: return major6;
            case MinorPent: return minorPent;
            case MajorPent: return majorPent;
            case Chromatic: return chromatic;
            default: return neutral6;
        }
    }
};
