#include "fft.hpp"
#include <cmath>
#include <gtest/gtest.h>

static constexpr float EPS = 1e-5f;
static constexpr float PI = M_PI;

FTransform fft;

static float energy(const std::vector<std::complex<float>> &v) {
    float e = 0.0f;
    for (const auto &c : v) {
        e += std::norm(c);
    }
    return e;
}

TEST(FFT, DCSignal_AllEnergyInBin0) {
    const std::size_t N = 8;
    float input[N];
    for (std::size_t i = 0; i < N; i++) {
        input[i] = 1.0f;
    }
    const auto out = fft.fft(input, N, false);
    EXPECT_NEAR(out[0].real(), N, EPS);
    for (std::size_t i = 1; i < out.size(); i++) {
        EXPECT_NEAR(std::abs(out[i]), 0.0f, EPS);
    }
}

TEST(FFT, Impulse_FlatSpectrum) {
    const std::size_t N = 8;
    float input[N] = {0};
    input[0] = 1.0f;
    const auto out = fft.fft(input, N, false);
    for (const auto &c : out) {
        EXPECT_NEAR(std::abs(c), 1.0f, EPS);
    }
}

TEST(FFT, SineWave_PeakAtCorrectBin) {
    const std::size_t N = 16;
    float input[N];
    const std::size_t k = 3;
    for (std::size_t i = 0; i < N; i++) {
        input[i] =
            std::sin(2.0f * PI * k * static_cast<float>(i) / static_cast<float>(N));
    }
    const auto out = fft.fft(input, N, false);
    std::size_t maxIdx = 0;
    float maxVal = 0.0f;
    for (std::size_t i = 0; i < out.size(); i++) {
        const float mag = std::abs(out[i]);
        if (mag > maxVal) {
            maxVal = mag;
            maxIdx = i;
        }
    }
    EXPECT_TRUE(maxIdx == k || maxIdx == N - k);
}

TEST(FFT, PadsToPowerOfTwo) {
    const std::size_t N = 10;
    float input[N];
    for (std::size_t i = 0; i < N; i++) {
        input[i] = static_cast<float>(i);
    }
    const auto out = fft.fft(input, N, false);
    EXPECT_EQ(out.size(), 16);
}

TEST(FFT, Inverse_Reconstruction) {
    const std::size_t N = 8;
    float input[N];
    for (std::size_t i = 0; i < N; i++) {
        input[i] = static_cast<float>(i + 1);
    }
    const auto freq = fft.fft(input, N, false);
    const auto inv = fft.fft(freq.data(), freq.size(), true);
    for (std::size_t i = 0; i < N; i++) {
        EXPECT_NEAR(inv[i].real(), input[i], EPS);
    }
}

TEST(FFT, ComplexInput_ForwardFFT) {
    const std::size_t N = 8;
    std::complex<float> input[N];
    for (std::size_t i = 0; i < N; i++) {
        input[i] = {static_cast<float>(i), 1.0f};
    }
    const auto out = fft.fft(input, N, false);
    EXPECT_EQ(out.size(), 8);
}

TEST(FFT, ComplexInput_InverseFFT) {
    const std::size_t N = 8;
    std::complex<float> input[N];
    for (std::size_t i = 0; i < N; i++) {
        input[i] = {static_cast<float>(i), 1.0f};
    }
    const auto freq = fft.fft(input, N, false);
    const auto inv = fft.fft(freq.data(), freq.size(), true);
    for (std::size_t i = 0; i < N; i++) {
        EXPECT_NEAR(inv[i].real(), input[i].real(), EPS);
        EXPECT_NEAR(inv[i].imag(), input[i].imag(), EPS);
    }
}

TEST(FFT, EdgeCase_N1) {
    float input[1] = {42.0f};
    const auto out = fft.fft(input, 1, false);
    EXPECT_EQ(out.size(), 1);
    EXPECT_NEAR(out[0].real(), 42.0f, EPS);
}

TEST(FFT, EdgeCase_N2) {
    float input[2] = {1.0f, -1.0f};
    const auto out = fft.fft(input, 2, false);
    EXPECT_EQ(out.size(), 2);
    EXPECT_GT(energy(out), 0.0f);
}

TEST(FFT, EdgeCase_NonPowerOfTwo) {
    const std::size_t N = 12;
    float input[N];
    for (std::size_t i = 0; i < N; i++) {
        input[i] = std::sin(static_cast<float>(i));
    }
    const auto out = fft.fft(input, N, false);
    EXPECT_EQ(out.size(), 16);
}

TEST(FFT, Energy_Conservation) {
    const std::size_t N = 8;
    float input[N];
    for (std::size_t i = 0; i < N; i++) {
        input[i] = std::sin(static_cast<float>(i));
    }
    const auto freq = fft.fft(input, N, false);
    float timeEnergy = 0.0f;
    for (std::size_t i = 0; i < N; i++) {
        timeEnergy += input[i] * input[i];
    }
    EXPECT_GT(energy(freq), 0.0f);
    EXPECT_NEAR(energy(freq) / static_cast<float>(freq.size()), timeEnergy, 1.0f);
}

TEST(FFT, RealInput_HasNonZeroSpectrum) {
    const std::size_t N = 8;
    float input[N];
    for (std::size_t i = 0; i < N; i++) {
        input[i] = static_cast<float>(i);
    }
    const auto out = fft.fft(input, N, false);
    bool any_nonzero = false;
    for (auto &c : out) {
        if (std::abs(c) > EPS) {
            any_nonzero = true;
        }
    }
    EXPECT_TRUE(any_nonzero);
}
