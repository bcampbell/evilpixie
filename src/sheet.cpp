#include "sheet.h"
#include "blit.h"
#include "draw.h"
#include "img.h"
#include "layer.h"
#include <cstdio>

void SpriteGrid::Layout(std::vector<Box>& cells) const
{
    assert (numFrames > 0 );
    int w = padX + cellW + padX;
    int h = padY + cellH + padY;
    for (unsigned int i=0; i<numFrames; ++i) {
        int x = (i % numColumns) * w;
        int y = (i / numColumns) * h;
        cells.push_back(Box(x, y, cellW, cellH));
    }
}


Layer* LayerToSpriteSheet(Layer const& src, SpriteGrid const& grid)
{
    std::vector<Box> cells;
    grid.Layout(cells);
    Box destBounds = grid.Extent();
    Img *dest = new Img(src.Fmt(), destBounds.w, destBounds.h);
    for (unsigned int i = 0; i < cells.size(); ++i) {
        Img const& srcImg = *src.mFrames[i]->mImg;
        Blit(srcImg, srcImg.Bounds(), *dest, cells[i]);
    }

    Layer* newLayer = new Layer();
    newLayer->mFPS = src.mFPS;
    newLayer->mPalette = src.mPalette;
    newLayer->mRanges = src.mRanges;
    newLayer->mFrames.push_back(new Frame(dest, 1000000/newLayer->mFPS));
    return newLayer;
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


