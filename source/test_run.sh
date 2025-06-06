#!/bin/bash

set -e

BASE_NAME=$(basename *.pro .pro)

if [ ! -d "../bin" ]; then
  mkdir ../bin
fi

cd ../bin

qmake6 ../source/*.pro
make clean
make

./"$BASE_NAME"
