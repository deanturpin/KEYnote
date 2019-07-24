# Build the example projects
Install dependencies.
```bash
sudo apt install -y \
g++-8 \
make \
libfreetype6-dev \
libxinerama-dev \
webkit2gtk-4.0
```

Fetch and build tutorials.
```bash
git clone --depth=1 https://github.com/deanturpin/juce-example-projects && \
cd juce-example-projects && \
./build-example-projects.sh
```

Run the tutorials sequentially.
```bash
./run-example-projects.sh
```

# Building the IDE
Get the source.
```bash
git clone --depth=1 https://github.com/WeAreROLI/JUCE && \
cd JUCE
```

Modify a flag in ```extras/Projucer/JuceLibraryCode/AppConfig.h``` so you can
run without a Roli account.
```c++
#ifndef JUCER_ENABLE_GPL_MODE
 #define JUCER_ENABLE_GPL_MODE 1
#endif
```

Build Projucer IDE and the examples.
```bash
for dir in extras/*; do make -j $(nproc) -C $dir/Builds/LinuxMakefile; done
```

Run Projucer.
```bash
extras/Projucer/Builds/LinuxMakefile/build/Projucer
```
