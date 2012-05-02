#!/bin/bash

# This script automatically calls expand and indent
# on either:
#
#  * The entire tree or
#  * The files changed since last commit (if invoked as pre-commit)
#
# You should make a symbolic link from .git/hooks/pre-commit
# to this file if you intend to make commits on this project.
#
# - DCPU Team

pushd "$(git rev-parse --show-toplevel)" >/dev/null

FILES=""
if [ "$(basename $0)" == "pre-commit" ]; then
    # Get a list of files from Git.
    TFILES="$(git diff --cached --name-only)"
    for i in $TFILES; do
        if [ "$(find $i \( -name '*.h' -or -name '*.c' -or -name '*.cpp' \) -and -not -path './third-party/*')" != "" ]; then
            FILES="$FILES$i "
        fi
    done
else
    # Search through the whole tree for relevant files.
    FILES=$(find ./ \( -name "*.h" -or -name "*.c" -or -name "*.cpp" \) -and -not -path "./third-party/*")
fi

# Sanitize all files.
for i in $FILES; do
    echo "Sanitizing $i..."

    # If we are running in pre-commit mode, modify the Git index
    # directly.
    if [ "$(basename $0)" == "pre-commit" ]; then
        # Get temporary files to store this stuff
        # in.    
        STORE=$(mktemp)
        OUTPUT=$(mktemp)

        # Extract the current state of the file from
        # the Git index and store it in $STORE
        git cat-file blob :$i > $STORE

        # Format our file.
        expand -i $STORE > $OUTPUT
        cat $OUTPUT > $STORE
        indent -i4 -bad -bap -bl -bli0 -cli4 -nut -cbi0 -ss -npcs -npsl -ncs -di0 -nbc -bls -blf -lp -ip2 -ppi0 -il0 -l1000 $STORE -o $OUTPUT
        cat $OUTPUT > $STORE

        # Add our current temporary storage file
        # back into the index under the original file
        # name.
        GIT_MODE=$(git ls-files -s | awk -v path=$i "\$0 ~ path{print \$1}")
        GIT_HASH=$(git hash-object -w --path="$i" $STORE)
        git update-index --cacheinfo $GIT_MODE $GIT_HASH $i

        # Clean up.
        rm $STORE
        rm $OUTPUT
    else
        # Format our file.
        expand -i $i > $i.fmtd
        mv $i.fmtd $i
        indent -i4 -bad -bap -bl -bli0 -cli4 -nut -cbi0 -ss -npcs -npsl -ncs -di0 -nbc -bls -blf -lp -ip2 -ppi0 -il0 -l1000 $i -o $i.fmtd
        mv $i.fmtd $i
    fi 
done

popd >/dev/null
exit 0
