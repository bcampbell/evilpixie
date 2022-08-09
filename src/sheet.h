#ifndef SHEET_H
#define SHEET_H

#include <vector>
#include "box.h"

class Img;
class Layer;

// Struct to describe how to lay out sprites on a regular grid.
struct SpriteGrid
{
    enum {ROW, COLUMN} orientation{ROW};
    unsigned int numColumns{1};
    unsigned int numRows{1};
    unsigned int hpad{0};
    unsigned int vpad{0};
    unsigned int totalFrames{1};
};

// fns for converting layouts between spritesheets and anims

// Works out boxes for laying out frame images in a grid.
// Returns the overall dimensions of the resultant layout.
Box LayoutSpritesheet(Layer const& srcLayer, SpriteGrid const& grid, std::vector<Box>& outFrames);


//
void UnpackSpriteGrid(Box const& srcBox, SpriteGrid const& grid, std::vector<Box>& frames);


Layer* LayerToSpriteSheet(Layer const& src, int nColumns);
void FramesFromSpriteSheet(Img const& src, int nWide, int nHigh, std::vector<Img*>& destFrames);


#endif // SHEET_H

