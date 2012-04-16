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
../$ASSEMBLER src/openf.dasm16 -o bin/openf.bin || exit 1
dd if=bin/openf.bin ibs=1 skip=24576 of=bin/openf_trim.bin || exit 1
../$ASSEMBLER src/echof.dasm16 -o bin/echof.bin || exit 1
dd if=bin/echof.bin ibs=1 skip=24576 of=bin/echof_trim.bin || exit 1
../$ASSEMBLER src/cleanf.dasm16 -o bin/cleanf.bin || exit 1
dd if=bin/cleanf.bin ibs=1 skip=24576 of=bin/cleanf_trim.bin || exit 1
../$ASSEMBLER src/aperture.dasm16 -o bin/aperture.bin || exit 1
dd if=bin/aperture.bin ibs=1 skip=24576 of=bin/aperture_trim.bin || exit 1

../$ASSEMBLER src/0x42c.dasm16 -o bin/0x42c.bin || exit 1
dd if=bin/0x42c.bin ibs=1 skip=8192 of=bin/0x42c_trim.bin || exit 1
cd src; ../../$ASSEMBLER kernel.dasm16 -o ../bin/kernel.bin || exit 1; cd ../
cp bin/kernel.bin dist/kernel.bin || exit 1

exit 0
