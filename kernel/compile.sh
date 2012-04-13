#!/usr/bin/env bash

if [ ! -d bin ]
	then mkdir bin;
fi
if [ ! -d dist ]
	then mkdir dist;
fi

# Change this if you're running UNIX.
ASSEMBLER=Debug/assembler.exe

# programs
../$ASSEMBLER src/openf.dasm16 -o bin/openf.bin
dd if=bin/openf.bin ibs=1 skip=24576 of=bin/openf_trim.bin
../$ASSEMBLER src/echof.dasm16 -o bin/echof.bin
dd if=bin/echof.bin ibs=1 skip=24576 of=bin/echof_trim.bin
../$ASSEMBLER src/cleanf.dasm16 -o bin/cleanf.bin
dd if=bin/cleanf.bin ibs=1 skip=24576 of=bin/cleanf_trim.bin
../$ASSEMBLER src/aperture.dasm16 -o bin/aperture.bin
dd if=bin/aperture.bin ibs=1 skip=24576 of=bin/aperture_trim.bin

../$ASSEMBLER src/0x42c.dasm16 -o bin/0x42c.bin
dd if=bin/0x42c.bin ibs=1 skip=8192 of=bin/0x42c_trim.bin
cd src; ../../$ASSEMBLER kernel.dasm16 -o ../bin/kernel.bin; cd ../
cp bin/kernel.bin dist/kernel.bin
