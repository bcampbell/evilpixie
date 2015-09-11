#ifndef SHEET_H
#define SHEET_H

#include <vector>
#include "box.h"

class Img;
class Anim;

// fns for converting layouts betweeen spritesheets and anims
Box LayoutSpritesheet(Anim const& src, int nWide, std::vector<Box>& frames);
void SplitSpritesheet(Box const& srcBox, int nWide, int nHigh, std::vector<Box>& frames);


// TODO: take frames as params
Img* GenerateSpriteSheet(Anim const& src, int maxAcross);
void FramesFromSpriteSheet(Img const& src, int nWide, int nHigh, std::vector<Img*>& destFrames);


#endif // SHEET_H

