# Install dependencies.
```bash
sudo apt install -y \
g++-8 \
make \
libfreetype6-dev \
libxinerama-dev \
webkit2gtk-4.0 \
ladspa-sdk
```

# Build the IDE and all example projects
- Clone the library
- Patch it so it Projucer will run without a Roli account
- Compile every project that contains a Linux build area

```bash
cd && \
git clone --depth=1 https://github.com/WeAreROLI/JUCE && \
cd JUCE && \
sed -i s/JUCER_ENABLE_GPL_MODE\ 0/JUCER_ENABLE_GPL_MODE\ 1/ extras/Projucer/JuceLibraryCode/AppConfig.h && \
for dir in $(find . -name LinuxMakefile); do make -j $(nproc) -C $dir; done
```

Run Projucer.
```bash
extras/Projucer/Builds/LinuxMakefile/build/Projucer
```
