#!/usr/bin/env bash

# Switch to Debug folder.
pushd ../Debug
./emulator.exe ../kernel/dist/kernel.bin
popd

