all:
	$(MAKE) -j $(shell nproc) --directory SpectrumAnalyserTutorial/Builds/LinuxMakefile
	SpectrumAnalyserTutorial/Builds/LinuxMakefile/build/SpectrumAnalyserTutorial
