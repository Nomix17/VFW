#!/bin/bash

set -e

BASE_NAME=$(basename *.pro .pro)
ASSETS_DIR="$HOME/.local/share/VFW"
CONFIGS_DIR="$HOME/.config/VFW"

createDirs() {
  mkdir -p ../bin "$ASSETS_DIR" "$CONFIGS_DIR"
}

createDirs

cp -r ../assets/. "$ASSETS_DIR"/
cp -r ../configs/. "$CONFIGS_DIR"/

cd ../bin

qmake6 ../source/*.pro
make clean
make

./"$BASE_NAME"
