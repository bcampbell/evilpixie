#ifndef SHEET_H
#define SHEET_H

#include <vector>
#include "box.h"

class Img;
class Layer;

// fns for converting layouts between spritesheets and anims
Box LayoutSpritesheet(Layer const& src, int nWide, std::vector<Box>& frames);
void SplitSpritesheet(Box const& srcBox, int nWide, int nHigh, std::vector<Box>& frames);


// TODO: take frames as params
Img* GenerateSpriteSheet(Layer const& src, int maxAcross);
void FramesFromSpriteSheet(Img const& src, int nWide, int nHigh, std::vector<Img*>& destFrames);


#endif // SHEET_H

