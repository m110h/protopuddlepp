# ProtoPuddle++
A simulator of cells world. It is a remake of [ProtoPuddle](https://github.com/dem2k/Protopuddle) (you can watch a video about the project [here](https://www.youtube.com/watch?v=a0xYF_c3GrM)) with an unique base of source code.

## Third Party
ProtoPuddle++ uses external projects enumerated below:
1. https://github.com/wxWidgets/wxWidgets/releases/tag/v3.1.3
2. https://github.com/effolkronium/random
3. https://github.com/nlohmann/json

## Build
Use cmake for building. At now, Windows and Linux had been tested.

### Linux (manjaro for example)

At first, you have to install a development environment:
```
$ sudo pacman -S gcc
$ sudo pacman -S make
$ sudo pacman -S gdb
$ sudo pacman -S git
$ sudo pacman -S cmake
```
Next, download an archive with wxWidgets 3.1.3 source code:

1) unpack the archive (tar xvjf *.tar,bz2)
2) cd {directory with a source code}
3) ./configure && make && sudo make install

Also you can try to install wxWidgets from a repository.

And final, build the project:
```
$ cd {some directory}
$ git clone https://github.com/m110h/protopuddlepp.git
$ cd protopuddlepp
$ mkdir build
$ cd build
$ cmake ..
$ make
```
## Binary
Binary releases for Windows 64 bit are available. Use this [link](https://github.com/m110h/protopuddlepp/releases).

## Screenshot
![Image description](screenshots/main_window.jpg)
