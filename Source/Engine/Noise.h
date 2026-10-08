#pragma once
#include <random>
#include <cstdint>

class Noise
{
public:
    Noise(uint32_t seed = 0x12345678)
    {
        state[0] = seed;
        state[1] = seed ^ 0x9E3779B9;
        state[2] = seed ^ 0x85EBCA6B;
        state[3] = seed ^ 0xC2B2AE35;
    }

    float nextWhite()
    {
        return nextUInt() * (1.0f / 4294967296.0f) * 2.0f - 1.0f;
    }

    float processBrown(float input)
    {
        brownAccum = brownAccum * 0.998f + input * 0.002f;
        return brownAccum;
    }

private:
    uint32_t state[4];
    float brownAccum = 0.0f;

    uint32_t nextUInt()
    {
        const uint32_t result = state[0] + state[3];
        const uint32_t t = state[1] << 9;

        state[2] ^= state[0];
        state[3] ^= state[1];
        state[1] ^= state[2];
        state[0] ^= state[3];

        state[2] ^= t;
        state[3] = rotl(state[3], 11);

        return result;
    }

    static uint32_t rotl(uint32_t x, int k)
    {
        return (x << k) | (x >> (32 - k));
    }
};
