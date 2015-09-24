#ifndef DRAW_H
#define DRAW_H

#include "colours.h"

class Img;
class Box;
class Point;
struct Palette;



//---------------------------------------------
// Blit fns (in draw_blit.cpp)


// helper
void clip_blit(
    Box const& srcbounds, Box& srcbox,
    Box const& destbounds, Box& destbox,
    int xzoom=1,
    int yzoom=1 );

void Blit( Img const& srcimg, Box const& srcbox, Img& destimg, Box& destbox);

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

// destbox is changed to reflect the final clipped area on the dest Img
void BlitTransparent( Img const& srcimg, Box const& srcbox, Palette const& srcpalette, Img& destimg, Box& destbox,
    PenColour const& transparentcolour );

// Same as Blit, except that srcimg is replaced by destimg
void BlitSwap( Img& srcimg, Box const& srcbox, Img& destimg, Box& destbox);

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


// blit an RGBA img, blending with src alpha onto the dest (must be RGBX8 or RGBA8)
void BlitRGBA8( Img const& srcimg, Box const& srcbox, Img& destimg, Box& destbox );

//---------------------------------------------
// BlitZoom fns (in draw_blitzoom.cpp)
//

// Blit with a integer zoom factor, and a transparent colour
// (palette required to blit from indexed to rgb image)
void BlitZoomKeyed( Img const& srcimg, Box const& srcbox,
    Palette const& srcpalette,
    Img& destimg, Box& destbox,
    int xzoom, int yzoom,
    PenColour const& transparentcolour);


// Blit with a integer zoom factor, and a transparent colour
void BlitZoomMatteKeyed( Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    int xzoom, int yzoom, PenColour const& transparentcolour, PenColour const& mattecolour );


void BlitZoom(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    Palette const& srcpalette,
    int xzoom,
    int yzoom );

void BlitZoomI8(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    Palette const& srcpalette,
    int xzoom,
    int yzoom );


//---------------------------------------------
// Drawing fns (in draw.cpp)
//

// sets damage to bounding rect for affected area
void FloodFill( Img& img, Point const& start, PenColour const& newcolour, Box& damage );

//
void RectFill(Img& destimg, Box& destbox, PenColour const& pen );

// Functions to walk along lines/ellipses, invoking a callback to do the actual drawing.
void WalkLine(int x0, int y0, int x1, int y1, void (*plot)(int x, int y, void* user ), void* userdata );
void WalkEllipse(int xc, int yc, int r1, int r2,
    void (*drawpixel)(int x, int y, void* user ), void* userdata );
void WalkFilledEllipse(int xc, int yc, int r1, int r2,
    void (*drawhline)(int x0, int x1, int y, void* user ), void* userdata );

#endif // DRAW_H

