#ifndef BLIT_MATTE_H_INCLUDED
#define BLIT_MATTE_H_INCLUDED

#include "colours.h"

class Img;
struct Box;
class Point;
struct Palette;

// Blit, but all non-transparent pixels are drawn using mattecolour, rather
// then being copied.
// destbox is changed to reflect the final clipped area on the dest Img
void BlitMatte( Img const& srcimg, Box const& srcbox, Img& destimg, Box& destbox,
    PenColour const& transparentcolour, PenColour const& mattecolour );

/*
// Blit matte of an I8 img onto another img, colourkeyed.
void BlitMatteI8Keyed(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    int transparentIdx,
    PenColour const& mattecolour );

void BlitMatteRGBX8Keyed(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    RGBX8 transparent,
    RGBA8 matte );

void BlitMatteRGBA8(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    RGBA8 matte );
*/

#endif // BLIT_MATTE_H_INCLUDED
