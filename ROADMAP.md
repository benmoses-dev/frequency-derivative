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

### Stage 2 - Visual Feedback
- Provide visual feedback on spectral features:
  - Addressable LED ring visualisation based on dominant frequencies or frequency delta - Done
  - Map MFCCs and spectral centroid onto the LED ring

---

