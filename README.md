![Slow motion](demo/slow-motion.gif)

"Disappear 3" is a KWin effect that animates the disappearing of windows.

## Installation

### Arch Linux

For Arch Linux [kwin-effects-disappear3](https://aur.archlinux.org/packages/kwin-effects-disappear3/)
is available in the AUR.

### Fedora

```sh
sudo dnf copr enable zzag/kwin-effects
sudo dnf refresh
sudo dnf install kwin-effects-disappear3
```

### Ubuntu

```sh
sudo add-apt-repository ppa:vladzzag/kwin-effects
sudo apt install libkwin4-effect-disappear3
```

### Build from source

#### Prerequisites

* Arch Linux
  ```sh
  sudo pacman -S cmake extra-cmake-modules kwin
  ```
* Fedora
  ```sh
  sudo dnf install cmake extra-cmake-modules kf5-kconfig-devel kf5-kcoreaddons-devel kf5-kwindowsystem-devel kwin-devel qt5-qtbase-devel
  ```
* Ubuntu
  ```sh
  sudo apt install cmake extra-cmake-modules kwin-dev libkf5config-dev libkf5coreaddons-dev libkf5windowsystem-dev qtbase5-dev
  ```

#### Build

```sh
git clone https://github.com/zzag/kwin-effects-disappear3.git
cd kwin-effects-disappear3
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
sudo make install
```
