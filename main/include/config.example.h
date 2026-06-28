#pragma once

#include <cstdint>

#define DEBUG 0

/**
 * Pin configuration
 */
inline constexpr std::uint32_t SCK_PIN = 26;
inline constexpr std::uint32_t WS_PIN = 25;
inline constexpr std::uint32_t SD_PIN = 33;
inline constexpr std::uint32_t LED_GPIO = 18;
/**
 * System configuration
 */
inline constexpr std::uint32_t SAMPLE_RATE =
    44100; // See the datasheet for your INMP mems mic
inline constexpr std::size_t BUFFER_SIZE =
    1024; // Higher provides more resolution but is slower
inline constexpr std::uint32_t LED_COUNT = 24;
inline constexpr std::uint32_t LED_RESOLUTION = 10'000'000;
inline constexpr std::uint32_t TOP_K = 3; // Average the frequency over the top K FFT bins
inline constexpr float GAIN = 0.02f;      // Higher = brighter LEDs for the same volume
inline constexpr float HPF_CUTOFF = 20.0f;
inline constexpr float LPF_CUTOFF = 8000.0f;
inline constexpr float MIN_MAG = 0.4f;  // Noise floor for background sound
inline constexpr float LED_FADE = 0.8f; // LED decay trail
/**
 * Frequency clamping.
 * Higher range = more resolution but harder to hit low and high frequencies.
 */
inline constexpr float MAX_FREQ = 4000.0f;
inline constexpr float MIN_FREQ = 120.0f;
