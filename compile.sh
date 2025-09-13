#!/bin/bash

# Check for -x flag
RUN_AFTER_COMPILE=false
if [ "$1" == "-x" ]; then
    RUN_AFTER_COMPILE=true
fi

# Compile all CPP files
g++ *.cpp -o gameoflife.out -lSDL2 -lSDL2_ttf

# If compilation was successful and -x is set, run the program
if $RUN_AFTER_COMPILE && [ $? -eq 0 ]; then
    ./gameoflife.out
fi
