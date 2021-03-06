#ifndef BLIT_RANGE_H_INCLUDED
#define BLIT_RANGE_H_INCLUDED

#include "colours.h"
#include <vector>

class Img;
struct Box;
class Point;
struct Palette;

// use srcimg as a mask to shift pixels in destimg up or down a colour range.
void BlitRangeShiftKeyed(Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    PenColour const& transparentcolour,
    std::vector<PenColour> const& range,
    int direction);

// rect will be clipped to destimg
void DrawRectRangeShift(Img& destimg, Box& rect, std::vector<PenColour> const& range, int direction);

#endif // BLIT_RANGE_H_INCLUDED

