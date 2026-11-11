# Fourier Series Visualizer (Qt + QtCharts)

[![ci](https://github.com/DYyyyDYyy/qt-fourier-series-visualizer/actions/workflows/ci.yml/badge.svg)](https://github.com/DYyyyDYyy/qt-fourier-series-visualizer/actions/workflows/ci.yml)

A Qt 6 desktop app that computes the Fourier series of a periodic signal and shows the original wave, its approximation and the amplitude spectrum.

- Signals: square pulses, sawtooth, parabolic wave
- Fourier coefficients computed by numerical integration — choose **midpoint rectangles**, **trapezoids** or **Simpson's rule**
- Configurable interval, number of harmonics and integration steps
- Charts: original vs. reconstructed signal (line chart) and spectrum `cₙ` (bar chart)

## Build

Requires Qt 6 with the **Charts** module and CMake ≥ 3.20.

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64
cmake --build build
./build/FourierSeriesLab
```
