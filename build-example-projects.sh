#!/bin/bash

# CPUs available to build with
cpus=$(nproc)

projects=(
# Audio
PlayingSoundFilesTutorial
ProcessingAudioInputTutorial
AudioThumbnailTutorial

# Synth
SynthUsingMidiInputTutorial
SimpleSynthNoiseTutorial
SynthLevelControlTutorial
WavetableSynthTutorial

# Interface design
FlexBoxGridTutorial

# DSP
DSPIntroductionTutorial
DSPDelayLineTutorial
DSPConvolutionTutorial
SIMDRegisterTutorial
SimpleFFTTutorial
SpectrumAnalyserTutorial

# Graphics
AnimationTutorial
MainComponentTutorial
GraphicsTutorial
OpenGLAppTutorial
)

# Fetch all projects
for project in ${projects[@]}; do

  # Fetch the project if it doesn't exist
  if [[ ! -d $project ]]; then
    curl --output $project.zip https://docs.juce.com/tutorials/ZIPs/$project.zip
    unzip $project.zip
    rm -f $project.zip
  fi
done

echo Applying patches
sed -i s/AnimationTutorial_01.h/AnimationTutorial_04.h/ AnimationTutorial/Source/Main.cpp
sed -i s/AudioThumbnailTutorial_01.h/AudioThumbnailTutorial_04.h/ AudioThumbnailTutorial/Source/Main.cpp
sed -i s/DSPConvolutionTutorial_01.h/DSPConvolutionTutorial_02.h/ DSPConvolutionTutorial/Source/Main.cpp
sed -i s/PlayingSoundFilesTutorial_01.h/PlayingSoundFilesTutorial_03.h/ PlayingSoundFilesTutorial/Source/Main.cpp
sed -i s/SIMDRegisterTutorial_01.h/SIMDRegisterTutorial_02.h/ SIMDRegisterTutorial/Source/Main.cpp
sed -i s/SimpleFFTTutorial_01.h/SimpleFFTTutorial_02.h/ SimpleFFTTutorial/Source/Main.cpp
sed -i s/SimpleSynthNoiseTutorial_01.h/SimpleSynthNoiseTutorial_03.h/ SimpleSynthNoiseTutorial/Source/Main.cpp
sed -i s/SpectrumAnalyserTutorial_01.h/SpectrumAnalyserTutorial_02.h/ SpectrumAnalyserTutorial/Source/Main.cpp
sed -i s/SynthLevelControlTutorial_01.h/SynthLevelControlTutorial_02.h/ SynthLevelControlTutorial/Source/Main.cpp
sed -i s/SynthUsingMidiInputTutorial_01.h/SynthUsingMidiInputTutorial_02.h/ SynthUsingMidiInputTutorial/Source/Main.cpp
sed -i s/WavetableSynthTutorial_01.h/WavetableSynthTutorial_04.h/ WavetableSynthTutorial/Source/Main.cpp

# Build all projects
for project in ${projects[@]}; do

  build_dir=$project/Builds/LinuxMakefile

  # Build for Linux if build area exists
  if [[ -d $build_dir ]]; then
    echo Building $project with $cpus CPUs
    make --directory $build_dir -j $cpus
  else
    echo $build_dir does not exist
  fi

done
