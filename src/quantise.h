#ifndef QUANTISE_H
#define QUANTISE_H

#include <vector>
#include "colours.h"

class Img;

void CalculatePalette(Img const& srcImg, std::vector<Colour>& out, int nColours);

#endif // QUANTISE_H
