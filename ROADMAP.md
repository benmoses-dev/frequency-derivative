# Project Roadmap

---

**Sensors:** INMP441 microphone, LEDs
**Platform:** ESP32

### Stage 1 - Feature Extraction
- Real-time DSP pipeline:
  - LPF, HPF - done
  - Windowing - done
  - FFT - done
  - Spectral features:
    - Dominant frequency - done
    - MFCCs
    - Spectral centroid, rolloff
    - Zero-crossing rate

### Stage 2 - Anomaly Detection
- Detect unusual audio events:
  - Alarms, machinery faults, glass breaking
- Statistical thresholds or autoencoder-based anomaly detection

### Stage 3 - Edge Audio Classification
- Train small ML models for classification on-device
- Classes could include:
  - Speech, background noise, alarms
  - Music, machinery, environmental sounds
- Integrate with real-time DSP

---

