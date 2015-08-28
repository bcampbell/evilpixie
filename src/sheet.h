#ifndef SHEET_H
#define SHEET_H

#include <vector>

class Img;
class Anim;

Img* GenerateSpriteSheet(Anim const& src, int maxAcross);
void FramesFromSpriteSheet(Img const& src, int nWide, int nHigh, std::vector<Img*>& destFrames);


#endif // SHEET_H

