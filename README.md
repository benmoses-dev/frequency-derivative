# Frequency Derivative

Real-time audio frequency visualiser for ESP32.

Audio from an INMP441 I2S microphone is processed with filtering,
windowing, and an FFT to determine the dominant frequencies, 
which are averaged and then mapped to colour on a WS2812B LED ring.

------------------------------------------------------------------------

## DSP Pipeline

```
INMP441
↓
High-pass filter (remove DC / rumble)
↓
Low-pass filter (reduce noise and bandwidth)
↓
Hann window
↓
FFT
↓
Top K dominant frequency detection
↓
Logarithmic scaling + gain
↓
Frequency + amplitude -> HSV colour
↓
HSV -> RGB
↓
WS2812B LED output with decay trail
```

------------------------------------------------------------------------

## Visualisation

Low frequencies appear **red**, mid frequencies **green**, and higher
frequencies **blue -> purple**.

------------------------------------------------------------------------

## Hardware

- **ESP32**
- **INMP441 I2S MEMS microphone**
- **WS2812B 24-LED ring**

Typical wiring:

| Device | ESP32 |
|-------|------|
| INMP441 SCK | I2S clock |
| INMP441 WS | I2S word select |
| INMP441 SD | I2S data |
| LED DIN | GPIO |
| LED VCC | 5V |
| LED GND | GND |

I set the pins as follows (Espressif DevKitC):

| Device | ESP32 |
|-------|------|
| INMP441 SCK | GPIO 26 |
| INMP441 WS | GPIO 25 |
| INMP441 SD | GPIO 33 |
| LED DIN | GPIO 18 |
| LED VCC | 5V |
| LED GND | GND |

------------------------------------------------------------------------

## Build

Requires **ESP-IDF**.

```bash
git clone https://github.com/benmoses-dev/frequency-derivative.git
cd frequency-derivative
idf.py get-dependencies
idf.py build
idf.py -p PORT flash monitor
```

------------------------------------------------------------------------

## Configuration

Key parameters are configurable in the code:

| Parameter | Description |
|-------|------|
| SAMPLE_RATE | I2S sampling rate for the MEMS microphone |
| BUFFER_SIZE | FFT size (higher gives finer resolution but impacts performance) |
| minFreq / maxFreq | Frequency range mapped to LEDs |
| fade | LED decay factor (trail) |
| gain | Magnitude scaling |

------------------------------------------------------------------------

## Notes:

- I have found that the current settings give a very good result, but feel free to get in touch with settings that you like!
- This project comes with a very high risk of your children singing very loudly.

------------------------------------------------------------------------

