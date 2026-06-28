#include "fft.hpp"
#include <cmath>

static constexpr float TAU = 2.0f * static_cast<float>(M_PI);

FTransform::FTransform() {}

/**
 * We can test the FFT using black box end-to-end tests.
 * Unit tests don't make as much sense, so we can hide these helper functions inside this
 * TU.
 */
static void radix2Pad(std::vector<std::complex<float>> &in) {
    const std::size_t N = in.size();
    if ((N & (N - 1)) != 0) {
        std::size_t p = 2;
        while (p < N) {
            p <<= 1;
        }
        const std::size_t diff = p - N;
        for (std::size_t i = 0; i < diff; i++) {
            in.emplace_back(0.0, 0.0);
        }
    }
}

static void bitReverse(std::vector<std::complex<float>> &in) {
    const std::size_t N = in.size();
    std::size_t j = 0;
    for (std::size_t i = 1; i < N; i++) {
        std::size_t msb = N >> 1;
        while (j & msb) {
            j ^= msb;
            msb >>= 1;
        }
        j ^= msb;
        if (i < j) {
            std::swap(in[i], in[j]);
        }
    }
}
static std::pair<std::complex<float>, std::complex<float>>
applyTwiddle(const std::complex<float> &even, const std::complex<float> &odd,
             const std::complex<float> &twiddle) {
    return {even + (odd * twiddle), even - (odd * twiddle)};
}

static void fftIt(std::vector<std::complex<float>> &in, const bool inverse) {
    const std::size_t N = in.size();
    if (N == 1) {
        return;
    }
    bitReverse(in);
    for (std::size_t length = 2; length <= N; length <<= 1) {
        const std::size_t half = length / 2;
        std::vector<std::complex<float>> twiddles(half);
        for (std::size_t k = 0; k < half; k++) {
            const float angle = TAU * static_cast<float>(k) / static_cast<float>(length) *
                                (inverse ? 1 : -1);
            twiddles[k] = {std::cos(angle), std::sin(angle)};
        }
        for (std::size_t i = 0; i < N; i += length) {
            for (std::size_t k = 0; k < half; k++) {
                const std::complex<float> &even = in[i + k];
                const std::complex<float> &odd = in[i + k + half];
                const auto [first, second] = applyTwiddle(even, odd, twiddles[k]);
                in[i + k] = first;
                in[i + k + half] = second;
            }
        }
    }
}

static std::vector<std::complex<float>>
fftInternal(std::vector<std::complex<float>> &spectrum, const bool inverse) {
    radix2Pad(spectrum);
    fftIt(spectrum, inverse);
    if (inverse) {
        for (auto &f : spectrum) {
            f /= static_cast<float>(spectrum.size());
        }
    }
    return spectrum;
}

std::vector<std::complex<float>> FTransform::fft(const float *input, const std::size_t N,
                                                 const bool inverse) const {
    std::vector<std::complex<float>> spectrum = normalise(input, N);
    return fftInternal(spectrum, inverse);
}

std::vector<std::complex<float>> FTransform::fft(const std::complex<float> *input,
                                                 const std::size_t N,
                                                 const bool inverse) const {
    std::vector<std::complex<float>> spectrum = normalise(input, N);
    return fftInternal(spectrum, inverse);
}
