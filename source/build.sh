#!/bin/bash

set -e

BASE_NAME=$(basename *.pro .pro)
ASSETS_DIR="$HOME/.local/share/VFW"
CONFIGS_DIR="$HOME/.config/VFW"
DESKTOP_ENTRIES_DIR="$HOME/.local/share/applications/"
ICONS_DIR="/usr/share/icons/hicolor/256x256/apps/"

createDirs() {
  mkdir -p ../bin "$ASSETS_DIR" "$CONFIGS_DIR" "$ICONS_DIR"
}

createDirs

cp -r ../assets/. "$ASSETS_DIR"/
cp -r ../configs/. "$CONFIGS_DIR"/
sudo cp ../assets/icons/VFW-Squircle-Dark.png "$ICONS_DIR/vfw.png"

cd ../bin

qmake6 ../source/*.pro
make clean
make

sudo cp ./VFW /usr/local/bin

cp ../VFW.desktop $DESKTOP_ENTRIES_DIR
