#ifndef DRAW_H
#define DRAW_H

#include "colours.h"

class Img;
struct Box;
class Point;
struct Palette;

// Drawing fns

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

