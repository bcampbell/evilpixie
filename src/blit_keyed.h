#ifndef BLIT_KEYED_H_INCLUDED
#define BLIT_KEYED_H_INCLUDED

#include "colours.h"

class Img;
struct Box;
class Point;
struct Palette;


// blit from an I8 source to any target, with colourkey transparency
void BlitI8Keyed(
    Img const& srcimg, Box const& srcbox,
    Palette const& srcpalette,
    Img& destimg, Box& destbox,
    int transparentIdx );

// blit from an RGBX8 source to any target, with colourkey transparency
void BlitRGBX8Keyed(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    RGBX8 transparent );

// destbox is changed to reflect the final clipped area on the dest Img
void BlitTransparent( Img const& srcimg, Box const& srcbox, Palette const& srcpalette, Img& destimg, Box& destbox,
    PenColour const& transparentcolour );


#endif // BLIT_KEYED_H_INCLUDED

