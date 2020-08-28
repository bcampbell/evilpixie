# EvilPixie

Homepage: <http://evilpixie.scumways.com>

## Overview

A pixel-oriented paint program. If you ever used Deluxe Paint, this should
be familiar :-)


## Requirements

Depends upon:

- qt5
- [libimpy](http://github.com/bcampbell/impy)
- libpng
- giflib (version 5)
- libjpeg

For Debian/Unbuntu, this means something like:
```
$ sudo apt install qt5-default libjpeg-dev libpng-dev libgif-dev
```

## Building from source

Under Linux, Mac and Windows (under msys2):

    $ git clone https://github.com/bcampbell/evilpixie.git
    $ cd evilpixie
    $ qmake evilpixie.pro
    $ make

Under Linux, you can then install it with:

    $ sudo make install

The app looks for its data files in `/usr/local/share/evilpixie` by
default. This can be annoying for development, so you can force
it to look in `./data` instead by hiding the `EVILPIXIE_DATA_DIR`
define. In `evilpixie.pro`, comment out the:

    DEFINES += EVILPIXIE_DATA_DIR=\\\"$${EVILPIXIE_DATA_DIR}\\\"


There is some basic support for Mac and Windows builds in `mac/`
and `win32/` respectively.

The type of build is set in evilpixie.pro:

    CONFIG += qt debug

or

    CONFIG += qt release

(TODO: figure out the proper way to pass the build type in on the `qmake`
command line)

