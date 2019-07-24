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
```bash
./build-example-projects.sh
```
