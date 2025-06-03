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

# mkdir -p /opt/VFW
# cp -r ../cache /opt/VFW/
# cp -r ../bin /opt/VFW/

# cp ../cache/run.sh /usr/bin/VFW

# cp ../cache/VFW.desktop $1/.local/share/applications
