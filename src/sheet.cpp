#include "sheet.h"
#include "blit.h"
#include "draw.h"
#include "img.h"
#include "layer.h"
#include "lexer.h"

#include <cstdio>

void SpriteGrid::Layout(std::vector<Box>& cells) const
{
    assert(cellW > 0);
    assert(cellH > 0);
    unsigned int cnt = (numFrames > 0) ? numFrames : numColumns * numRows;
    int w = padX + cellW + padX;
    int h = padY + cellH + padY;
    for (unsigned int i = 0; i < cnt; ++i) {
        int x = (i % numColumns) * w;
        int y = (i / numColumns) * h;
        cells.push_back(Box(x, y, cellW, cellH));
    }
}


std::string SpriteGrid::Stringify(Box const& imgbounds) const
{
    std::string out;
    if (numFrames < 2) {
        return out;
    }

    auto append = [&](std::string name, int val) {
        if (!out.empty()) {
            out += " ";
        }
        out += name + "=" + std::to_string(val);
    };

    if (numColumns != 1) {
        append("cols",numColumns);
    }
    if (numRows != 1) {
        append("rows",numRows);
    }
    if (padX > 0) {
        append("xpad",padX);
    }
    if (padY > 0) {
        append("ypad",padY);
    }
    // Don't write cellw and cellh if they can be inferred from the overall
    // image size.
    bool exactW = (numColumns * (padX + cellW + padX) == (unsigned int)imgbounds.w);
    if (!exactW) {
        append("w",cellW);
    }
    bool exactH = (numRows * (padY + cellH + padY) == (unsigned int)imgbounds.h);
    if (!exactH) {
        append("h",cellH);
    }
    // Don't write framecount if it can be easily inferred.
    if (numFrames != numColumns * numRows) {
        append("frames",numFrames);
    }
    return out;
}


bool SpriteGrid::Parse(std::string input, Box const& imgbounds)
{
    Lexer lexer(input);

    // Defaults
    numColumns = 1;
    numRows = 1;
    padX = 0;
    padY = 0;
    // Stuff we'll derive if not explicit in input.
    numFrames = 0;
    cellW = 0;
    cellH = 0;

    std::string ident;
    int n;
    while(ParseNumericAssignment(lexer, ident, n)) {
        if (ident == "cols") {
            numColumns = (unsigned int)n;
        } else if (ident == "rows") {
            numRows = (unsigned int)n;
        } else if (ident == "xpad") {
            padX = (unsigned int)n;
        } else if (ident == "ypad") {
            padY = (unsigned int)n;
        } else if (ident == "w") {
            cellW = (unsigned int)n;
        } else if (ident == "h") {
            cellH = (unsigned int)n;
        } else if (ident == "frames") {
            numFrames = (unsigned int)n;
        } else {
            // Ignore unknown fields.
        }
    }

    if (cellW == 0) {
        cellW = (imgbounds.w / numColumns) - (padX * 2);
    }
    if (cellH == 0) {
        cellH = (imgbounds.h / numRows) - (padY * 2);
    }
    if (numFrames == 0) {
        numFrames = numColumns * numRows;
    }

    return true;
}


Img* FramesToSpriteSheet(std::vector<Frame*> const& frames, SpriteGrid const& grid)
{
    assert(!frames.empty());
    std::vector<Box> cells;
    grid.Layout(cells);
    Box destBounds = grid.Extent();
    Img *dest = new Img(frames[0]->mImg->Fmt(), destBounds.w, destBounds.h);
    for (unsigned int i = 0; i < cells.size(); ++i) {
        Img const& srcImg = *frames[i]->mImg;
        Blit(srcImg, srcImg.Bounds(), *dest, cells[i]);
    }
    return dest;
}


// split up a sprite sheet into multiple frames
void FramesFromSpriteSheet(Img const& src, SpriteGrid const& grid, std::vector<Img*>& destFrames)
{
    std::vector<Box> cells;
    grid.Layout(cells);

    for(auto cell : cells) {
        Img* dest = new Img(src.Fmt(), cell.w, cell.h);
        Box destBox = dest->Bounds();
        Blit(src, cell, *dest, destBox);
        destFrames.push_back(dest);
    }
}


