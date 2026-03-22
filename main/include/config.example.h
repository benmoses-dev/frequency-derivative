#pragma once

/**
 * Pin configuration
 */
#define SCK_PIN 26
#define WS_PIN 25
#define SD_PIN 33
#define LED_GPIO 18
/**
 * System configuration
 */
#define SAMPLE_RATE 44100 // See the datasheet for your INMP mems mic
#define BUFFER_SIZE 1024  // Higher provides more resolution but is slower
#define LED_COUNT 24
#define LED_RESOLUTION 10'000'000
#define TOP_K 3                      // Average the frequency over the top K FFT bins
static constexpr float GAIN = 0.02f; // Higher = brighter LEDs for the same volume
static constexpr float HPF_CUTOFF = 20.0f;
static constexpr float LPF_CUTOFF = 8000.0f;
static constexpr float MIN_MAG = 0.4f;  // Noise floor for background sound
static constexpr float LED_FADE = 0.8f; // LED decay trail
/**
 * Frequency clamping.
 * Higher range = more resolution but harder to hit low and high frequencies.
 */
static constexpr float MAX_FREQ = 4000.0f;
static constexpr float MIN_FREQ = 120.0f;
