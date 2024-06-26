#ifndef SHEET_H
#define SHEET_H

#include <vector>
#include <string>
#include <cassert>

#include "box.h"

class Img;
class Layer;
class Frame;

// Struct to describe how to lay out sprites on a regular grid.
struct SpriteGrid
{
//    enum {ROW, COLUMN} orientation{ROW};
    unsigned int numColumns{1};
    unsigned int numRows{1};
    unsigned int padX{0};
    unsigned int padY{0};

    unsigned int cellW{0};  // width of each cell (exclude padX) 0=unset
    unsigned int cellH{0};  // height of each cell (excluding padY) 0=unset
    unsigned int numFrames{1};  // <= rows * cols

    // Calculate cells in the grid. Returns overall bounding box.
    void Layout(std::vector<Box>& cells) const;

    // Return the overall size of the layout.
    Box Extent() const
    {
        assert(numColumns > 0);
        assert(numRows > 0);

        assert(cellW > 0);
        assert(cellH > 0);
        const int w = padX + cellW + padX;
        const int h = padY + cellH + padY;
        return Box(0, 0, numColumns * w, numRows * h);
    }

    std::string Stringify(Box const& imgbounds) const;
    bool Parse(std::string input, Box const& imgbounds);
};


// fns for converting layouts between spritesheets and anims


// FramesToSpriteSheet() creates a spritesheet image from a sequence of frames.
// The frames are laid out according to the grid.
Img* FramesToSpriteSheet(std::vector<Frame*> const& frames, SpriteGrid const& grid);

void FramesFromSpriteSheet(Img const& src, SpriteGrid const& grid, std::vector<Img*>& destFrames);


#endif // SHEET_H

