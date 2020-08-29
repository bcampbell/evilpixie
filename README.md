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
    $ meson setup build
    $ meson compile -C build

Under Linux, you can then install it with:

    $ meson install -C build

By default the install prefix is /usr/local, but you can specify a different install location during setup:

    $meson setup --prefix /tmp/epinstall build


