#include "sheet.h"
#include "draw.h"
#include "img.h"
#include "anim.h"
#include <cstdio>



// TODO: should return a set of bounding boxes for the offets
Img* GenerateSpriteSheet(Anim const& src, int maxAcross)
{
    int nWide = src.NumFrames();
    int nHigh = 1;
    if(maxAcross>0 && nWide>maxAcross) {
        nHigh = (nWide+(maxAcross-1)) / maxAcross;
        nWide = maxAcross;
    }

    // TODO: handle differing frame offsets

    Box bounds;
    src.CalcBounds(bounds,0,src.NumFrames());
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
            Img const& srcImg = src.GetFrameConst(frame);
            Box destBox(x*frameW, y*frameH, srcImg.W(), srcImg.H());
            Blit(srcImg, srcImg.Bounds(), *dest, destBox);
        }
    }
    return dest;
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


