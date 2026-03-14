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
    - Spectral centroid, rolloff - skipped
    - Top K weighted average frequency - done
    - MFCCs

### Stage 2 - Visual Feedback
- Provide visual feedback on spectral features:
  - Addressable LED ring visualisation based on dominant frequencies or frequency delta - done
  - Map MFCCs onto the LED ring

---

