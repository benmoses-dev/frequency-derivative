#include "fft.hpp"
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

static constexpr float PI = M_PIf;

static float sink = 0.0f;

std::vector<float> makeSignal(const std::size_t N) {
    std::vector<float> signal(N);
    const float n = static_cast<float>(N);
    for (std::size_t i = 0; i < N; i++) {
        signal[i] = 0.5f * std::sin(2.0f * PI * 5.0f * static_cast<float>(i) / n) +
                    0.25f * std::sin(2.0f * PI * 17.0f * static_cast<float>(i) / n) +
                    0.1f * std::sin(2.0f * PI * 37.0f * static_cast<float>(i) / n);
    }
    return signal;
}

void benchmark(const std::size_t N, const std::size_t iterations) {
    FTransform fft;
    const auto signal = makeSignal(N);
    for (std::size_t i = 0; i < 100; i++) {
        const auto out = fft.fft(signal.data(), signal.size(), false);
        sink += out[0].real();
    }
    const auto start = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iterations; i++) {
        const auto out = fft.fft(signal.data(), signal.size(), false);
        sink += out[0].real();
    }
    const auto end = std::chrono::high_resolution_clock::now();
    const auto us =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    const double per = static_cast<double>(us) / static_cast<double>(iterations);
    std::cout << "N=" << N << " iterations=" << iterations << " total_us=" << us
              << " per_fft_us=" << per << '\n';
}

int main() {
    benchmark(256, 10000);
    benchmark(512, 5000);
    benchmark(1024, 2000);
    benchmark(2048, 1000);
    std::cout << sink << '\n';
    return 0;
}
