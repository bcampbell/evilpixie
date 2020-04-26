#ifndef BLIT_ZOOM_H_INCLUDED
#define BLIT_ZOOM_H_INCLUDED

#include "colours.h"

class Img;
struct Box;
class Point;
struct Palette;


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

#endif // BLIT_ZOOM_H_INCLUDED

