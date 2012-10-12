#ifndef DRAW_H
#define DRAW_H

#include "colours.h"

class Img;
class Box;
class Point;
class Palette;

void Blit( Img const& srcimg, Box const& srcbox, Img& destimg, Box& destbox);


// Blit, but all non-transparent pixels are drawn using mattecolour, rather
// then being copied.
// destbox is changed to reflect the final clipped area on the dest Img
void BlitMatte( Img const& srcimg, Box const& srcbox, Img& destimg, Box& destbox,
    PenColour const& transparentcolour, PenColour const& mattecolour );

// destbox is changed to reflect the final clipped area on the dest Img
void BlitTransparent( Img const& srcimg, Box const& srcbox, Img& destimg, Box& destbox,
    PenColour const& transparentcolour );

// Same as Blit, except that srcimg is replaced by destimg
void BlitSwap( Img& srcimg, Box const& srcbox, Img& destimg, Box& destbox);


// Blit with a integer zoom factor, and a transparent colour
// (palette required to blit from indexed to rgb image)
void BlitZoomTransparent( Img const& srcimg, Box const& srcbox, Img& destimg, Box& destbox,
    Palette const& palette, int zoom,
    PenColour const& transparentcolour);


// Blit with a integer zoom factor, and a transparent colour
void BlitZoomMatte( Img const& srcimg, Box const& srcbox, Img& destimg, Box& destbox,
    int zoom, PenColour const& transparentcolour, PenColour const& mattecolour );


// sets damage to bounding rect for affected area
void FloodFill( Img& img, Point const& start, PenColour const& newcolour, Box& damage );

// Functions to walk along lines/ellipses, invoking a callback to do the actual drawing.
void WalkLine(int x0, int y0, int x1, int y1, void (*plot)(int x, int y, void* user ), void* userdata );
void WalkEllipse(int xc, int yc, int r1, int r2,
    void (*drawpixel)(int x, int y, void* user ), void* userdata );
void WalkFilledEllipse(int xc, int yc, int r1, int r2,
    void (*drawhline)(int x0, int x1, int y, void* user ), void* userdata );

#endif // DRAW_H

