all:
	$(MAKE) -j $(shell nproc) --directory src/Builds/LinuxMakefile
	src/Builds/LinuxMakefile/build/SpectrumAnalyserTutorial
