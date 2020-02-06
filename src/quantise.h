#ifndef QUANTISE_H
#define QUANTISE_H

#include <vector>
#include "colours.h"
class Palette;
class Img;

void CalculatePalette(Img const& srcImg, std::vector<Colour>& out, int nColours, Palette const* srcPalette = nullptr);

#endif // QUANTISE_H
