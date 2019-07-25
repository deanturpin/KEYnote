/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             SpectrumAnalyserTutorial
 version:          2.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Displays an FFT spectrum analyser.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_processors, juce_audio_utils, juce_core,
                   juce_data_structures, juce_dsp, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2017, linux_make

 type:             Component
 mainClass:        AnalyserComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

const std::map<double, std::string> notes{

    // Catch all for lower bound search
    {0.0, "Bx"},

    // All the notes we're interested in. At the low end a single Fourier bin
    // will map to multiple notes. Further up multiple bins will be assigned
    // the same note.
    {16.35, "C0"},
    {17.32, "C#0"},
    {18.35, "D0"},
    {19.45, "D#0"},
    {20.60, "E0"},
    {21.83, "F0"},
    {23.12, "F#0"},
    {24.50, "G0"},
    {25.96, "G#0"},
    {27.50, "A0"},
    {29.14, "A#0"},
    {30.87, "B0"},
    {32.70, "C1"},
    {34.65, "C#1"},
    {36.71, "D1"},
    {38.89, "D#1"},
    {41.20, "E1"},
    {43.65, "F1"},
    {46.25, "F#1"},
    {49.00, "G1"},
    {51.91, "G#1"},
    {55.00, "A1"},
    {58.27, "A#1"},
    {61.74, "B1"},
    {65.41, "C2"},
    {69.30, "C#2"},
    {73.42, "D2"},
    {77.78, "D#2"},
    {82.41, "E2"},
    {87.31, "F2"},
    {92.50, "F#2"},
    {98.00, "G2"},
    {103.83, "G#2"},
    {110.00, "A2"},
    {116.54, "A#2"},
    {123.47, "B2"},
    {130.81, "C3"},
    {138.59, "C#3"},
    {146.83, "D3"},
    {155.56, "D#3"},
    {164.81, "E3"},
    {174.61, "F3"},
    {185.00, "F#3"},
    {196.00, "G3"},
    {207.65, "G#3"},
    {220.00, "A3"},
    {233.08, "A#3"},
    {246.94, "B3"},
    {261.63, "C4"},
    {277.18, "C#4"},
    {293.66, "D4"},
    {311.13, "D#4"},
    {329.63, "E4"},
    {349.23, "F4"},
    {369.99, "F#4"},
    {392.00, "G4"},
    {415.30, "G#4"},
    {440.00, "A4"},
    {466.16, "A#4"},
    {493.88, "B4"},
    {523.25, "C5"},
    {554.37, "C#5"},
    {587.33, "D5"},
    {622.25, "D#5"},
    {659.25, "E5"},
    {698.46, "F5"},
    {739.99, "F#5"},
    {783.99, "G5"},
    {830.61, "G#5"},
    {880.00, "A5"},
    {932.33, "A#5"},
    {987.77, "B5"},
    {1046.50, "C6"},
    {1108.73, "C#6"},
    {1174.66, "D6"},
    {1244.51, "D#6"},
    {1318.51, "E6"},
    {1396.91, "F6"},
    {1479.98, "F#6"},
    {1567.98, "G6"},
    {1661.22, "G#6"},
    {1760.00, "A6"},
    {1864.66, "A#6"},
    {1975.53, "B6"},
    {2093.00, "C7"},
    {2217.46, "C#7"},
    {2349.32, "D7"},
    {2489.02, "D#7"},
    {2637.02, "E7"},
    {2793.83, "F7"},
    {2959.96, "F#7"},
    {3135.96, "G7"},
    {3322.44, "G#7"},
    {3520.00, "A7"},
    {3729.31, "A#7"},
    {3951.07, "B7"},
    {4186.01, "C8"},
    {4434.92, "C#8"},
    {4698.63, "D8"},
    {4978.03, "D#8"},
    {5274.04, "E8"},
    {5587.65, "F8"},
    {5919.91, "F#8"},
    {6271.93, "G8"},
    {6644.88, "G#8"},
    {7040.00, "A8"},
    {7458.62, "A#8"},
    {7902.13, "B8"},
};

//==============================================================================
class AnalyserComponent : public AudioAppComponent, private Timer {
public:
  AnalyserComponent()
      : forwardFFT(fftOrder),
        window(fftSize, dsp::WindowingFunction<float>::blackman) {
    setOpaque(true);
    setAudioChannels(2, 0); // we want a couple of input channels but no outputs
    startTimerHz(30);
    setSize(600, 400);
  }

  ~AnalyserComponent() { shutdownAudio(); }

  //==============================================================================
  void prepareToPlay(int, double) override {}
  void releaseResources() override {}

  void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override {
    if (bufferToFill.buffer->getNumChannels() > 0) {
      auto *channelData =
          bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

      for (auto i = 0; i < bufferToFill.numSamples; ++i)
        pushNextSampleIntoFifo(channelData[i]);
    }
  }

  //==============================================================================
  void paint(Graphics &g) override {
    g.fillAll(Colours::black);

    g.setOpacity(1.0f);
    g.setColour(Colours::white);
    drawFrame(g);
  }

  void timerCallback() override {
    if (nextFFTBlockReady) {
      drawNextFrameOfSpectrum();
      nextFFTBlockReady = false;
      repaint();
    }
  }

  void pushNextSampleIntoFifo(float sample) noexcept {
    // if the fifo contains enough data, set a flag to say
    // that the next frame should now be rendered..
    if (fifoIndex == fftSize) {
      if (!nextFFTBlockReady) {
        zeromem(fftData, sizeof(fftData));
        memcpy(fftData, fifo, sizeof(fifo));
        nextFFTBlockReady = true;
      }

      fifoIndex = 0;
    }

    fifo[fifoIndex++] = sample;
  }

  void drawNextFrameOfSpectrum() {

    // first apply a windowing function to our data
    window.multiplyWithWindowingTable(fftData, fftSize);

    // then render our FFT data..
    forwardFFT.performFrequencyOnlyForwardTransform(fftData);

    auto mindB = -100.0f;
    auto maxdB = 0.0f;

    for (int i = 0; i < scopeSize; ++i) {
      auto skewedProportionX =
          1.0f - std::exp(std::log(1.0f - i / (float)scopeSize) * 0.2f);
      auto fftDataIndex =
          jlimit(0, fftSize / 2, (int)(skewedProportionX * fftSize / 2));
      auto level = jmap(jlimit(mindB, maxdB,
                               Decibels::gainToDecibels(fftData[fftDataIndex]) -
                                   Decibels::gainToDecibels((float)fftSize)),
                        mindB, maxdB, 0.0f, 1.0f);

      scopeData[i] = level;
    }
  }

  void drawFrame(Graphics &g) {


    for (int i = 1; i < scopeSize; ++i) {

      const auto width = getLocalBounds().getWidth();
      const auto height = getLocalBounds().getHeight();

      g.drawLine({(float)jmap(i - 1, 0, scopeSize - 1, 0, width),
                  jmap(scopeData[i - 1], 0.0f, 1.0f, (float)height * 2, 0.0f),
                  (float)jmap(i, 0, scopeSize - 1, 0, width),
                  jmap(scopeData[i], 0.0f, 1.0f, (float)height, 0.0f)});
    }

    // Get largest bin for this frame
    const auto it =
        std::max_element(std::cbegin(scopeData), std::cend(scopeData));
    const size_t current_peak_bin = std::distance(std::cbegin(scopeData), it);

    // Default note
    static std::string peak_note = "NA";

    // Create a histogram of recent bins
    static size_t iterations{0};
    static std::map<size_t, size_t> peaks;

    // Store recent bins and calculate the 
    if (iterations < 20) {
      ++iterations;
      ++peaks[current_peak_bin];
    } else {

      // Find the greatest bin
      const auto max_bin = *std::max_element(
        peaks.cbegin(), peaks.cend(),
        [](const auto &a, const auto &b){
          return a.second < b.second;
        }
        );

      for (const auto &[key, value] : peaks)
        std::cout << key << "\t" << value << "\n";

      // Calculate frequency of bin
      const double resolution = 44100 / (fftSize * 2);
      const double frequency = max_bin.first * resolution / 4;

      std::cout << max_bin.first << " max bin "
        << frequency << " frequency "
        << resolution << " resolution\n";

      // Find the closest note for the bin
      for (auto i = notes.cbegin(); i != notes.cend(); ++i)
        if (i->first > frequency) {

          // Calculate the gaps between the bin frequency and nearest notes
          const double lowerGap = frequency - std::prev(i)->first;
          const double upperGap = i->first - frequency;

          // Report the note closest to the bin frequency
          if (lowerGap > upperGap)
            peak_note = i->second;
          else
            peak_note = std::prev(i)->second;

          break;
        }

      // Clear down peaks for next time
      iterations = 0;
      peaks.clear();
    }

    // Draw the current frequency
    g.setFont(60);
    g.setColour(Colours::red);
    g.drawText(peak_note, getLocalBounds(), Justification::centred, true);
  }

  enum { fftOrder = 11, fftSize = 1 << fftOrder, scopeSize = 512 };

private:
  dsp::FFT forwardFFT;
  dsp::WindowingFunction<float> window;

  float fifo[fftSize];
  float fftData[2 * fftSize];
  int fifoIndex = 0;
  bool nextFFTBlockReady = false;
  float scopeData[scopeSize];

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyserComponent)
};
