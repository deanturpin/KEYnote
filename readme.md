# Install dependencies
```bash
sudo apt install -y \
libfreetype6-dev \
libxinerama-dev \
webkit2gtk-4.0
```

# Configure
```bash
git clone --depth=1 https://github.com/WeAreROLI/JUCE
cd JUCE
```

## Change this flag so you can run without a Roli account
extras/Projucer/JuceLibraryCode/AppConfig.h
```c++
#ifndef JUCER_ENABLE_GPL_MODE
 #define JUCER_ENABLE_GPL_MODE 1
#endif
```

# Build the Projucer IDE and all the examples
```bash
for dir in extras/*; do make -j $(nproc) -C $dir/Builds/LinuxMakefile; done
```

# Run Projucer
```bash
extras/Projucer/Builds/LinuxMakefile/build/Projucer
```

# Example projects
## Spectrum Analyser Tutorial
```bash
curl https://docs.juce.com/tutorials/ZIPs/SpectrumAnalyserTutorial.zip --output SpectrumAnalyserTutorial.zip
unzip SpectrumAnalyserTutorial.zip
```

Edit main to include the 02 header (the one with the code changes according to the instructions).
```bash
SpectrumAnalyserTutorial/Source/Main.cpp
```
Build and run it.
```bash
make -j $(nproc) -C SpectrumAnalyserTutorial/Builds/LinuxMakefile/
SpectrumAnalyserTutorial/Builds/LinuxMakefile/build/SpectrumAnalyserTutorial
```

## Simple FFT Tutorial
- Fetch the source
- Unzip it
- Replace the include with the complete project
- Build
- Run
```bash
curl https://docs.juce.com/tutorials/ZIPs/SimpleFFTTutorial.zip --output SimpleFFTTutorial.zip
unzip SimpleFFTTutorial.zip
sed -i s/SimpleFFTTutorial_01.h/SimpleFFTTutorial_02.h/ SimpleFFTTutorial/Source/Main.cpp
make -j $(nproc) --directory SimpleFFTTutorial/Builds/LinuxMakefile/
SimpleFFTTutorial/Builds/LinuxMakefile/build/SimpleFFTTutorial
```
