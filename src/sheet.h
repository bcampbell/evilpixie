#ifndef SHEET_H
#define SHEET_H

#include <vector>
#include "box.h"

class Img;
class Anim;

Box LayoutSpritesheet(Anim const& src, int nWide, std::vector<Box>& frames);


Img* GenerateSpriteSheet(Anim const& src, int maxAcross);
void FramesFromSpriteSheet(Img const& src, int nWide, int nHigh, std::vector<Img*>& destFrames);


#endif // SHEET_H

