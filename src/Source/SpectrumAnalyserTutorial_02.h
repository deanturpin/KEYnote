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

#include "notes.h"

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

    // Draw spectrum
    for (int i = 1; i < scopeSize; ++i) {

      const auto width = getLocalBounds().getWidth();
      const auto height = getLocalBounds().getHeight();

      g.drawLine({(float)jmap(i - 1, 0, scopeSize - 1, 0, width),
                  jmap(scopeData[i - 1], 0.0f, 1.0f, (float)height * 2, 0.0f),
                  (float)jmap(i, 0, scopeSize - 1, 0, width),
                  jmap(scopeData[i], 0.0f, 1.0f, (float)height, 0.0f)});
    }

    // Get largest FFT bin for this frame
    const auto it = std::max_element(std::cbegin(fftData), std::cend(fftData));
    const size_t current_peak_bin = std::distance(std::cbegin(fftData), it);

    // Default note (before we've measured anything)
    static std::string peak_note = "hello";

    // Create a histogram of recent peaks
    static size_t iterations{0};
    static std::map<size_t, size_t> peaks;

    // Store current bin and repeat for a few iterations to smooth out noisy
    // results
    if (iterations < 20) {
      ++iterations;

      // Ignore the noisy lower bins
      if (current_peak_bin > 10)
        ++peaks[current_peak_bin];

    } else {

      // Once the batch is complete find the greatest bin
      const auto max_bin = std::max_element(peaks.cbegin(), peaks.cend(),
                                            [](const auto &a, const auto &b) {
                                              return a.second < b.second;
                                            })
                               ->first;

      // Calculate frequency of bin
      const double resolution = 44100 / fftSize;
      const double frequency = (max_bin + 1) * resolution;

      // Find the closest note for the bin
      for (auto i = notes.cbegin(); i != notes.cend(); ++i)
        if (i->first > frequency) {

          // Calculate the gaps between the bin frequency and nearest notes
          const double lowerGap = frequency - std::prev(i)->first;
          const double upperGap = i->first - frequency;

          // Report the note closest to the bin frequency
          peak_note = (lowerGap > upperGap ? peak_note = i->second
                                           : peak_note = std::prev(i)->second) +
                      " | " + std::to_string(static_cast<uint32_t>(frequency));

          break;
        }

      // Clear down peaks for next time
      iterations = 0;
      peaks.clear();
    }

    // Display the calculated frequency and note
    g.setFont(60);
    g.setColour(Colours::blue);
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
