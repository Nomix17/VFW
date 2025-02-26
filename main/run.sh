#!/bin/bash

set -e 

BASE_NAME=$(basename *.pro .pro)

if [ ! -d "build" ]; then
    mkdir build
fi

cd build

qmake6 ../*.pro
make clean
make
./"$BASE_NAME"
