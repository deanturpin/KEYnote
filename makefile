all: build

build:
	$(MAKE) -j $(shell nproc) --directory src/Builds/LinuxMakefile

run: build
	src/Builds/LinuxMakefile/build/SpectrumAnalyserTutorial
