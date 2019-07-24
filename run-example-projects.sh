#!/bin/bash

# Run each project if there's an executable
for exe in */Builds/LinuxMakefile/build/*Tutorial; do
    echo Running ${exe##*/}
    $exe
done
