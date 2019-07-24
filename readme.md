# Compiling the dev environment

## Dependencies
```bash
sudo apt install -y \
libfreetype6-dev \
libxinerama-dev \
webkit2gtk-4.0
```

# Build Projucer
```bash
git clone --depth=1 https://github.com/WeAreROLI/JUCE
cd JUCE/extras/Projucer/Builds/LinuxMakefile
make -j $(nproc)
```
