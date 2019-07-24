#!/bin/bash

# CPUs available to build with
cpus=$(nproc)

projects=(
# Audio
SynthUsingMidiInputTutorial
SimpleSynthNoiseTutorial

# DSP
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
sed -i s/SimpleFFTTutorial_01.h/SimpleFFTTutorial_02.h/ SimpleFFTTutorial/Source/Main.cpp
sed -i s/AnimationTutorial_01.h/AnimationTutorial_04.h/ AnimationTutorial/Source/Main.cpp
sed -i s/SpectrumAnalyserTutorial_01.h/SpectrumAnalyserTutorial_02.h/ SpectrumAnalyserTutorial/Source/Main.cpp

# Build all projects
for project in ${projects[@]}; do

  build_dir=$project/Builds/LinuxMakefile

  # Build for Linux if build area exists
  if [[ -d $build_dir ]]; then
    echo Building $project with $cpus CPUs
    make -B --directory $build_dir -j $cpus
  else
    echo $build_dir does not exist
  fi

done

# Run each project if there's an executable
for project in ${projects[@]}; do

  exe=$project/Builds/LinuxMakefile/build/$project

  if [[ -f $exe ]]; then
    echo Running $project
    $exe
  fi

done
