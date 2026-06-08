#include "config.h"
#include "dsp.hpp"
#include <gtest/gtest.h>

DSP dsp(SAMPLE_RATE);
constexpr double EPS = 1e-6;

TEST(DSP, RMS_Simple) {
    float buffer[] = {3.0f, 4.0f};
    const auto res = dsp.computeRMS(buffer, 2);
    EXPECT_NEAR(res, sqrtf(12.5), EPS);
}

TEST(DSP, RMS_AllZero) {
    float buffer[100];
    for (std::size_t i = 0; i < 100; ++i) {
        buffer[i] = 0.0f;
    }
    const auto res = dsp.computeRMS(buffer, 100);
    EXPECT_NEAR(res, 0.0f, EPS);
}

TEST(DSP, RMS_AllOne) {
    float buffer[100];
    for (std::size_t i = 0; i < 100; ++i) {
        buffer[i] = 1.0f;
    }
    const auto res = dsp.computeRMS(buffer, 100);
    EXPECT_NEAR(res, 1.0f, EPS);
}

TEST(DSP, RMS_AllFive) {
    float buffer[100];
    for (std::size_t i = 0; i < 100; ++i) {
        buffer[i] = 5.0f;
    }
    const auto res = dsp.computeRMS(buffer, 100);
    EXPECT_NEAR(res, 5.0f, EPS);
}

TEST(DSP, LPF_SweepResponse) {
    constexpr std::size_t N = 2048;
    auto response = [&](float freq) {
        float x[N];
        float y[N];
        for (std::size_t i = 0; i < N; ++i) {
            x[i] = sinf(2 * static_cast<float>(M_PI) * freq * static_cast<float>(i) / static_cast<float>(SAMPLE_RATE));
            y[i] = sinf(2 * static_cast<float>(M_PI) * freq * static_cast<float>(i) / static_cast<float>(SAMPLE_RATE));
        }
        dsp.lpf(y, N, 1000.0f);
        return dsp.computeRMS(y, N) / dsp.computeRMS(x, N);
    };
    EXPECT_GT(response(50),   0.99f);
    EXPECT_GT(response(200),  0.95f);
    EXPECT_LT(response(2000), 0.5f);
    EXPECT_LT(response(8000), 0.2f);
}

TEST(DSP, HPF_FrequencyResponse) {
    constexpr std::size_t N = 4096;
    auto sine = [&](float f, float *buffer) {
        for (std::size_t i = 0; i < N; ++i) {
            buffer[i] = sinf(2 * static_cast<float>(M_PI) * f * static_cast<float>(i) / static_cast<float>(SAMPLE_RATE));
        }
    };
    auto gain = [&](float *x, float *y) {
        return dsp.computeRMS(y, N) / dsp.computeRMS(x, N);
    };
    float lowX[N], midX[N], highX[N];
    float lowY[N], midY[N], highY[N];
    sine(50.0f, lowX);
    sine(50.0f, lowY);
    sine(1000.0f, midX);
    sine(1000.0f, midY);
    sine(8000.0f, highX);
    sine(8000.0f, highY);
    dsp.hpf(lowY, N, 1000.0f);
    dsp.hpf(midY, N, 1000.0f);
    dsp.hpf(highY, N, 1000.0f);
    EXPECT_LT(gain(lowX,  lowY), 0.1);
    EXPECT_GT(gain(midX,  midY), 0.6);
    EXPECT_GT(gain(highX, highY), 0.9);
}


TEST(DSP, Hann_Definition) {
    constexpr std::size_t N = 8;
    float buffer[8];
    for (std::size_t i = 0; i < N; ++i) {
        buffer[i] = 1.0f;
    }
    dsp.hann(buffer, N);
    for (std::size_t n = 0; n < N; ++n) {
        const float expected =
            2.0f * 0.5f * (1.0f - cosf(2.0f * static_cast<float>(M_PI) * static_cast<float>(n) / static_cast<float>(N - 1)));
        EXPECT_NEAR(buffer[n], expected, EPS);
    }
}

// Todo: process() tests.