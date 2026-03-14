# Audio Processing Loop Profiling

This document summarises the performance of the audio processing loop on the ESP32, including time spent at each stage.

| Stage                         | Time (µs) | % of total loop (~23,000 µs) |
|--------------------------------|------------|-----------------------------|
| Audio read + HPF + LPF + Hann | 12,850     | 56%                        |
| FFT                            | 6,730      | 29%                        |
| Top-K processing               | 1,690      | 7%                         |
| Average calculations / hue     | 570        | 2%                         |
| Decay                          | 30         | 0%                         |
| LED output                     | 1,130      | 5%                         |
| **Total**                      | **23,000** | 100%                       |

