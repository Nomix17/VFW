# VFW Media Player
> A lightweight, theme-able media player for Linux and Windows, inspired by VLC. Built with Qt6 and C++17.

## Features
- Broad media format support (video & audio)
- Subtitle support — SRT, ASS/SSA, VTT formats
- Multiple audio track selection
- Chapter navigation with visual timeline markers
- Resume playback from last position
- Fully themeable UI via CSS files

## Getting a Binary
Pre-built binaries for Linux and Windows are available on the [Releases page](https://github.com/Nomix17/VFW/releases). Download the latest release and run it.

## Building from Source

### Requirements
**- Compiler & Build Tools**
- ***CMake ≥ 3.16***
- ***C++ compiler with C++17 support (GCC, Clang, or MSVC)***
- ***`ffprobe` (required for chapter detection)***
**- Qt6 Libraries**
- ***Qt6 Core, Widgets, Multimedia***
**- FFmpeg** (Qt Multimedia backend)

###  Installing Dependencies

#### - Ubuntu / Debian :
```bash
sudo apt install cmake g++ qt6-base-dev qt6-multimedia-dev qt6-tools-dev ffmpeg
```
**H.265/HEVC and extended codec support:**
```bash
sudo apt install libavcodec-extra ubuntu-restricted-extras
```
**Hardware acceleration (optional but recommended for 4K/H.265):**
```bash
# Intel
sudo apt install intel-media-va-driver vainfo
# AMD
sudo apt install mesa-va-drivers vainfo
# Nvidia
sudo apt install vdpau-driver-all
```

#### - Fedora :
```bash
sudo dnf install cmake gcc-c++ qt6-qtbase-devel qt6-qtmultimedia-devel qt6-qttools-devel ffmpeg
```
**H.265/HEVC and extended codec support (requires RPM Fusion):**
```bash
sudo dnf install https://mirrors.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm
sudo dnf install https://mirrors.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm
sudo dnf install ffmpeg --allowerasing
```
**Hardware acceleration (optional but recommended for 4K/H.265):**
```bash
# Intel
sudo dnf install intel-media-driver libva-utils
# AMD
sudo dnf install mesa-va-drivers libva-utils
# Nvidia
sudo dnf install nvidia-vaapi-driver
```

#### - Arch Linux :
```bash
sudo pacman -S cmake gcc qt6-base qt6-multimedia qt6-tools ffmpeg
```

**Hardware acceleration (optional but recommended for 4K/H.265):**
```bash
# Intel
sudo pacman -S intel-media-driver libva-utils
# AMD
sudo pacman -S libva-mesa-driver libva-utils
# Nvidia
sudo pacman -S nvidia-utils
```

#### - openSUSE :
```bash
sudo zypper install cmake gcc-c++ qt6-base-devel qt6-multimedia-devel qt6-tools-devel ffmpeg
```
**H.265/HEVC and extended codec support (requires Packman repository):**
```bash
sudo zypper ar -cfp 90 'https://ftp.gwdg.de/pub/linux/misc/packman/suse/openSUSE_Tumbleweed/' packman
sudo zypper refresh
sudo zypper dup --from packman --allow-vendor-change
```

**Hardware acceleration (optional but recommended for 4K/H.265):**
```bash
# Intel
sudo zypper install intel-media-driver libva2 vainfo
# AMD
sudo zypper install mesa-dri-drivers libva2 vainfo
# Nvidia
sudo zypper install nvidia-video-G06 libva2
```

### Build & Install
#### - To install:
```bash
git clone https://github.com/Nomix17/VFW
cd VFW
chmod +x deployment/build.sh deployment/uninstall.sh
cd deployment
./build.sh
```
#### - To uninstall:
```bash
cd deployment
./uninstall.sh
```

## Customization
Themes are plain CSS files located in `assets/themes/`. To create or modify a theme, edit any file there or add a new `.css` file alongside the existing ones. The structure is straightforward, no build step needed for theme changes.

## Project Structure
```
VFW/
├── assets/
│   ├── fonts/            # Bundled fonts (Montserrat, Arial, etc.)
│   ├── icons/            # Icon sets per theme (Dark, Light, Pitch-Black, Dark-Breeze)
│   └── themes/           # CSS theme files (Dark, Light, Pitch-Black, Dark-Breeze)
├── configs/              # Runtime configuration (settings, shortcuts, theme, subconfig)
├── deployment/
│   ├── build.sh          # Build & install script
│   ├── uninstall.sh      # Uninstall script
│   └── packaging/        # Windows installer files (.iss, .reg)
├── source/
│   ├── headers/          # C++ headers (main, UI components, dialog windows)
│   ├── main.cpp
│   └── mainwindow.cpp
├── CMakeLists.txt
└── VFW.desktop           # Linux desktop entry
```

## Contributing
Issues and pull requests are welcome. If you encounter a bug or want to request a feature, open an issue on the [GitHub repository](https://github.com/Nomix17/VFW/issues).

## License
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

VFW is licensed under the [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0). You are free to use, modify, and distribute this software, but any derivative work must also be released under the same license and remain open source.
