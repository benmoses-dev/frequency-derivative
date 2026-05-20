#include "dsp.hpp"
#include <algorithm>
#include <cmath>

DSP::DSP(const std::uint32_t sample_rate_)
    : dt_(1.0f / static_cast<float>(sample_rate_)) {}

DSP::~DSP() {}

std::pair<float, float>
DSP::process(const std::vector<std::complex<float>> &spectrum) const {
    const std::size_t N = spectrum.size();
    const float binHz = static_cast<float>(SAMPLE_RATE) / static_cast<float>(N);

    float maxMag = 0.0f;
    std::vector<Bin> bins;
    bins.reserve((N / 2) - 2);
    /**
     * Skip DC, Nyquist.
     * Potentially also skip the first frequency bin (experiment).
     */
    for (std::size_t i = 2; i < N / 2; i++) {
        /**
         * Don't account for the FFT dot product for now
         * Account for the negative complex conjugate (optional)
         */
        const float mag = std::fabs(spectrum[i]) * 2.0f;
        /**
         * topK max-heap.
         */
        const float freq = i * binHz;
        bins.emplace_back(mag, freq);
        maxMag = std::max(maxMag, mag);
    }
    std::sort(bins.begin(), bins.end());

    float weightedSum = 0.0f;
    float normSum = 0.0f;
    float magSum = 0.0f;
    std::int32_t counted = 0;
    for (std::size_t i = std::max(bins.size() - TOP_K, 0UL); i < bins.size(); i++) {
        const auto &[mag, freq] = bins[i];
        /**
         * Experiment here - we may not want to normalise.
         * Also play around with the cutoff noise-floor
         */
        const float normMag = mag / maxMag;
        if (normMag < 0.2) {
            continue;
        }
        counted++;
        weightedSum += freq * normMag;
        normSum += normMag;
        magSum += mag;
    }

    float avgMag = counted > 0 ? (magSum / static_cast<float>(counted)) : 0.0f;
    const float C = (normSum > 0.0f) ? (weightedSum / normSum) : 0.0f;
    const float spectralCentroid = std::fmax(std::fmin(C, MAX_FREQ), MIN_FREQ);
    const float numerator = std::log(spectralCentroid) - std::log(MIN_FREQ);
    const float denominator = std::log(MAX_FREQ) - std::log(MIN_FREQ);
    const float hue = std::fmin(numerator / denominator, 0.95f);
    if (avgMag < MIN_MAG) {
        avgMag = 0.0f;
    }
    float value = std::log1p(avgMag * GAIN);
    value = std::fmin(value, 1.0f);
    return {value, hue};
}

float DSP::computeRMS(const std::size_t numSamples, std::int32_t *data_buffer_) const {
    if (!data_buffer_ || numSamples == 0) {
        return 0.0;
    }
    float sum = 0;
    for (std::size_t i = 0; i < numSamples; i++) {
        const float sample = data_buffer_[i] >> 8;
        sum += sample * sample;
    }
    return sqrtf(sum / numSamples);
}

void DSP::hpf(float *buffer, const std::size_t N, const float cutoffHz) const {
    if (N == 0) {
        return;
    }
    const float RC = 1.0f / (TAU * cutoffHz);
    const float alpha = RC / (RC + dt_);
    float prevX = buffer[0];
    float prevY = 0.0f;
    for (std::size_t i = 0; i < N; i++) {
        const float x = buffer[i];
        const float deltaX = x - prevX;
        /**
         * Smoothed derivative to attenuate the low frequency signals
         */
        const float y = alpha * (prevY + deltaX);
        buffer[i] = y;
        prevX = x;
        prevY = y;
    }
}

void DSP::lpf(float *buffer, const std::size_t N, const float cutoffHz) const {
    if (N == 0) {
        return;
    }
    const float RC = 1.0f / (TAU * cutoffHz);
    const float alpha = dt_ / (RC + dt_);
    float prevY = buffer[0];
    for (std::size_t i = 0; i < N; i++) {
        const float x = buffer[i];
        /**
         * Exponential moving average
         */
        const float y = alpha * x + (1.0f - alpha) * prevY;
        buffer[i] = y;
        prevY = y;
    }
}

/**
 * Hann window to taper the signal to reduce spectral leakage before FFT.
 */
void DSP::hann(float *buffer, const std::size_t N) const {
    for (std::size_t i = 0; i < N; i++) {
        const float w = 0.5f * (1.0f - cosf(TAU * i / (N - 1)));
        buffer[i] *= (w * 2.0f); // account for amplitude reduction
    }
}
