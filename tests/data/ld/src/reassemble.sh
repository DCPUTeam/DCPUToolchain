#!/bin/bash

cd $(dirname "${BASH_SOURCE[0]}")
for i in *.dasm16; do
	name=${i%.dasm16}
	../../../../../0x10c-build/dtasm/dtasm -qqq -i -o ../${name}.i ${i}
	if [ $? -ne 0 ]; then
		echo "Assembling test ${name}.. failed."
	else
		echo "Assembling test ${name}.. done."
	fi
done
