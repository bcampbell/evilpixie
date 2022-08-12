#ifndef SHEET_H
#define SHEET_H

#include <vector>
#include "box.h"

class Img;
class Layer;

// Struct to describe how to lay out sprites on a regular grid.
struct SpriteGrid
{
//    enum {ROW, COLUMN} orientation{ROW};
    unsigned int numColumns{1};
    unsigned int numRows{1};
    unsigned int cellW{0};  // width of each cell (exclude padX)
    unsigned int cellH{0};  // height of each cell (excluding padY)
    unsigned int padX{0};
    unsigned int padY{0};
    unsigned int numFrames{1};

    // Calculate cells in the grid. Returns overall bounding box.
    void Layout(std::vector<Box>& cells) const;

    // Return the overall size of the layout.
    Box Extent() const {
        const int w = padX + cellW + padX;
        const int h = padY + cellH + padY;
        return Box(0, 0, numColumns * w, numRows * h);
    }

    void SubdivideBox(Box const& src, int cols, int rows)
    {
        numColumns = cols;
        numRows = rows;
        cellW = (src.w / cols)-(padX*2);
        cellH = (src.h / rows)-(padY*2);
        numFrames = cols * rows;
    }
};


// fns for converting layouts between spritesheets and anims


// LayerToSpriteSheet() creates a spritesheet layer from a multi-frame layer.
// The frames are laid out according to the grid.
Layer* LayerToSpriteSheet(Layer const& src, SpriteGrid const& grid);

void FramesFromSpriteSheet(Img const& src, SpriteGrid const& grid, std::vector<Img*>& destFrames);


#endif // SHEET_H

