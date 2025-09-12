## Dependencies

Before using this code, please ensure you have the following dependencies installed:

- **Qt6**: Required for the GUI framework
  
### Installting Dependencies 

#### Ubuntu/Debian
```bash
sudo apt install git qt6-base-dev qt6-multimedia-dev libgl1-mesa-dev ffmpeg 
```

#### Fedora
```bash
sudo dnf install git qt6-qtbase-devel qt6-qtmultimedia qt6-qtmultimedia-devel mesa-libGL-devel ffmpeg
```

#### Arch Linux
```bash
sudo pacman -S git qt6-base qt6-multimedia ffmpeg
```

## Installation

```bash
git clone https://github.com/Nomix17/VFW
cd VFW
chmod +x source/build.sh cache/run.sh source/uninstall.sh
cd source
sudo ./build.sh
sudo chown -R $(whoami):$(whoami) /opt/VFW
```
## Customization

To Edit or/and add a theme check the directory ```cache/styles/```.   
It's easy to customize, just some css files.
