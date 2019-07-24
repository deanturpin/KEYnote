#!/bin/bash

zips=(
OpenGLAppTutorial.zip
SimpleFFTTutorial.zip
SpectrumAnalyserTutorial.zip
)

for zip in ${zips[@]}; do
  curl --output $zip https://docs.juce.com/tutorials/ZIPs/$zip
  unzip $zip
  rm -f $zip
done
