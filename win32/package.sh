#!/bin/bash
set -e
# generate a .msi installer from a build (in ./evilpixie)

# data.wxs was originally auto-generated, then some dir structure hand-fiddled.
# original generation was:
#   heat dir evilpixie -gg -srd -sreg -suid -dr INSTALLDIR -cg grp_data -out data.wxs


candle -arch x64 main.wxs data.wxs
light main.wixobj data.wixobj -b evilpixie -o evilpixie-0.2.1.msi

