#ifndef DRAW_H
#define DRAW_H

#include "colours.h"

class Img;
class Box;
class Point;

void FloodFill( Img& img, Point const& start, PenColour const& newcolour, Box& damage );
void FloodFill_I8( Img& img, Point const& start, I8 newcolour, Box& damage );
void FloodFill_RGBX8( Img& img, Point const& start, RGBX8 newcolour, Box& damage );


#endif // DRAW_H

