#include "sheet.h"
#include "blit.h"
#include "draw.h"
#include "img.h"
#include "layer.h"
#include <cstdio>

Box LayoutSpritesheet(Layer const& srcLayer, SpriteGrid const& grid, std::vector<Box>& outFrames)
{
    assert(srcLayer.NumFrames() > 0);
    assert(outFrames.empty());

    // calc max frame size
    Box contain(0, 0, 0, 0);
    for (auto f : srcLayer.mFrames) {
        contain.Merge(f->mImg->Bounds());
    }
    assert(!contain.Empty());

    // lay out frames
    Box extent(0, 0, 0, 0);
    const unsigned int w = contain.w + (grid.hpad * 2);
    const unsigned int h = contain.h + (grid.vpad * 2);
    int n = 0;
    for (auto f : srcLayer.mFrames) {
        Point offset(
            grid.hpad + (n % grid.numColumns) * w,
            grid.vpad + (n / grid.numColumns) * h);
        Box b(f->mImg->Bounds() );
        b += offset;
        outFrames.push_back(b);
        extent.Merge(b);
        ++n;
        //printf("%d: %d,%d %dx%d\n", n, b.x, b.y, b.w, b.h);
    }

    // Include padding in total extent.
    extent.x -= grid.hpad;
    extent.y -= grid.vpad;
    extent.w += grid.hpad*2;
    extent.h += grid.vpad*2;

    //printf("extent: %d,%d %dx%d\n", extent.x, extent.y, extent.w, extent.h);
    return extent;
}




void UnpackSpriteGrid(Box const& srcBox, SpriteGrid const& grid, std::vector<Box>& frames)
{
    assert(frames.empty());

    int nFrames = grid.totalFrames;
    int frameW = srcBox.w / grid.numColumns;
    int frameH = srcBox.h / grid.numRows;

    int f;
    for( f=0; f<nFrames; ++f)
    {
        int x = f % grid.numColumns;
        int y = f / grid.numColumns;
        Box b(x*frameW, y*frameH, frameW, frameH);
        frames.push_back(b);
    }
}


// TODO: should return a set of bounding boxes for the offsets?
Layer* LayerToSpriteSheet(Layer const& src, int nColumns)
{
    assert(nColumns>0);
    int nWide = src.NumFrames();
    int nHigh = 1;
    if(nWide>nColumns) {
        nHigh = (nWide+(nColumns-1)) / nColumns;
        nWide = nColumns;
    }

    // TODO: handle differing frame offsets

    Box bounds = src.Bounds();
    int frameW = bounds.W();
    int frameH = bounds.H();

    Img *dest = new Img(src.Fmt(), frameW*nWide, frameH*nHigh);
    int frame = 0;
    int y;
    for( y=0; y<nHigh; ++y)
    {
        int x;
        for( x=0; x<nWide; ++x,++frame)
        {
            if(frame>=src.NumFrames())
                continue;
            Img const& srcImg = src.GetImgConst(frame);
            Box destBox(x*frameW, y*frameH, srcImg.W(), srcImg.H());
            Blit(srcImg, srcImg.Bounds(), *dest, destBox);
        }
    }

    Layer* newLayer = new Layer();
    newLayer->mFPS = src.mFPS;
    newLayer->mPalette = src.mPalette;
    newLayer->mRanges = src.mRanges;
    newLayer->mFrames.push_back(new Frame(dest, 1000000/newLayer->mFPS));
    return newLayer;
}


// split up a sprite sheet into multiple frames
void FramesFromSpriteSheet(Img const& src, int nWide, int nFrames, std::vector<Img*>& destFrames)
{
   int nHigh = (nFrames+nWide-1)/nWide; 
    int frameW = src.W()/nWide;
    int frameH = src.H()/nHigh;
    int n=0;
    int y;
    for(y=0; y<nHigh && n<nFrames; ++y)
    {
        int x;
        for(x=0; x<nWide && n<nFrames; ++x,++n)
        {
            Img* dest = new Img(src.Fmt(), frameW, frameH);
            Box destBox = dest->Bounds();
            Box srcBox(x*frameW, y*frameH, frameW, frameH);
            Blit(src,srcBox,*dest,destBox);
            destFrames.push_back(dest);
        }
    }
}


