# Project Roadmap

---

**Sensors:** INMP441 microphone, LEDs
**Platform:** ESP32

### Stage 1 - Feature Extraction
- Implement real-time DSP pipeline:
  - LPF, HPF - done
  - Windowing - done
  - FFT - done
  - Spectral features:
    - MFCCs
    - Spectral centroid, rolloff
    - Zero-crossing rate

### Stage 2 - Anomaly Detection
- Detect unusual audio events:
  - Alarms, machinery faults, glass breaking
- Statistical thresholds or autoencoder-based anomaly detection
- Logging for verification

### Stage 3 - Edge AI Audio Classification
- Train small ML models for classification on-device
- Classes could include:
  - Speech, background noise, alarms
  - Music, machinery, environmental sounds
- Integrate with real-time DSP pipeline
- Prioritise **real-time and low-level constraints** where applicable.

---

