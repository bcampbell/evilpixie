#include "sheet.h"
#include "blit.h"
#include "draw.h"
#include "img.h"
#include "layer.h"
#include <cstdio>

Box LayoutSpritesheet(Layer const& src, int nWide, std::vector<Box>& frames)
{
    assert(src.NumFrames()>0);
    assert(frames.empty());

    // calc max frame size
    int f;
    Box contain(0, 0, 0, 0);
    for (f=0; f<src.NumFrames(); ++f)
    {
        Box const& b = src.GetImgConst(f).Bounds();
        contain.Merge(b);
    }
    assert(!contain.Empty());

    // lay out frames

    Box  extent(0, 0, 0, 0);
    for (f=0; f<src.NumFrames(); ++f)
    {
        Point offset(
            (f % nWide) * contain.w,
            (f / nWide) * contain.h );
        Box b( src.GetImgConst(f).Bounds() );
        b += offset;
        frames.push_back(b);
        extent.Merge(b);
    }

    return extent;
}




void SplitSpritesheet(Box const& srcBox, int nWide, int nHigh, std::vector<Box>& frames)
{
    assert(frames.empty());

    int nFrames = nWide*nHigh;
    int frameW = srcBox.w / nWide;
    int frameH = srcBox.h / nHigh;

    int f;
    for( f=0; f<nFrames; ++f)
    {
        int x = f%nWide;
        int y = f/nWide;
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
    Frame* f = new Frame();
    f->mDuration = 1000000/newLayer->mFPS;
    f->mImg = dest;
    newLayer->mFrames.push_back(f);
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


