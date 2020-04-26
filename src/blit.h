#ifndef BLIT_H_INCLUDED
#define BLIT_H_INCLUDED

#include "colours.h"

class Img;
struct Box;
class Point;
struct Palette;

// helper
void clip_blit(
    Box const& srcbounds, Box& srcbox,
    Box const& destbounds, Box& destbox,
    int xzoom=1,
    int yzoom=1);

// Blit between images of the same format.
void Blit(Img const& srcimg, Box const& srcbox, Img& destimg, Box& destbox);

// Same as Blit, except that srcimg is replaced by destimg
void BlitSwap( Img& srcimg, Box const& srcbox, Img& destimg, Box& destbox);


// blit an RGBA img, blending with src alpha onto the dest (must be RGBX8 or RGBA8)
//void BlitRGBA8( Img const& srcimg, Box const& srcbox, Img& destimg, Box& destbox );


#endif // BLIT_H_INCLUDED

