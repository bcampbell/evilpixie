#include "colours.h"
#include "img.h"
#include "palette.h"
#include <cassert>


Img* ConvertRGBA8toI8(Img const& srcImg, Palette const& destPalette) {
    assert(srcImg.Fmt() == FMT_RGBA8);
    Img* destImg = new Img(FMT_I8, srcImg.W(), srcImg.H());

    for (int y=0; y<srcImg.H(); ++y) {
        const RGBA8 *src = srcImg.PtrConst_RGBA8(0,y);
        I8 *dest = destImg->Ptr_I8(0,y);
        for (int x=0; x<srcImg.W(); ++x) {

            *dest++ = (I8)destPalette.Closest(Colour(*src));
            ++src;
        }
    }
    return destImg;
}

Img* ConvertRGBX8toI8(Img const& srcImg, Palette const& destPalette) {
    assert(srcImg.Fmt() == FMT_RGBX8);
    Img* destImg = new Img(FMT_I8, srcImg.W(), srcImg.H());

    for (int y=0; y<srcImg.H(); ++y) {
        const RGBX8 *src = srcImg.PtrConst_RGBX8(0,y);
        I8 *dest = destImg->Ptr_I8(0,y);
        for (int x=0; x<srcImg.W(); ++x) {
            *dest++ = (I8)destPalette.Closest(Colour(*src));
            ++src;
        }
    }
    return destImg;
}

Img* ConvertI8toRGBX8(Img const& srcImg, Palette const& srcPalette) {
    assert(srcImg.Fmt() == FMT_I8);
    Img* destImg = new Img(FMT_RGBX8, srcImg.W(), srcImg.H());

    for (int y=0; y<srcImg.H(); ++y) {
        const I8 *src = srcImg.PtrConst_I8(0,y);
        RGBX8 *dest = destImg->Ptr_RGBX8(0,y);
        for (int x=0; x<srcImg.W(); ++x) {
            const Colour c = srcPalette.GetColour(*src);
            ++src;
            *dest++ = RGBX8(c.r, c.g, c.b);
        }
    }
    return destImg;
}

Img* ConvertI8toRGBA8(Img const& srcImg, Palette const& srcPalette) {
    assert(srcImg.Fmt() == FMT_I8);
    Img* destImg = new Img(FMT_RGBA8, srcImg.W(), srcImg.H());

    for (int y=0; y<srcImg.H(); ++y) {
        const I8 *src = srcImg.PtrConst_I8(0,y);
        RGBA8 *dest = destImg->Ptr_RGBA8(0,y);
        for (int x=0; x<srcImg.W(); ++x) {
            const Colour c = srcPalette.GetColour(*src);
            ++src;
            *dest++ = RGBA8(c.r, c.g, c.b, c.a);
        }
    }
    return destImg;
}


Img* ConvertRGBA8toRGBX8(Img const& srcImg) {
    assert(srcImg.Fmt() == FMT_RGBA8);
    Img* destImg = new Img(FMT_RGBX8, srcImg.W(), srcImg.H());

    for (int y=0; y<srcImg.H(); ++y) {
        const RGBA8 *src = srcImg.PtrConst_RGBA8(0,y);
        RGBX8 *dest = destImg->Ptr_RGBX8(0,y);
        for (int x=0; x<srcImg.W(); ++x) {
            *dest++ = Colour(*src++);
        }
    }
    return destImg;
}

Img* ConvertRGBX8toRGBA8(Img const& srcImg) {
    assert(srcImg.Fmt() == FMT_RGBX8);
    Img* destImg = new Img(FMT_RGBA8, srcImg.W(), srcImg.H());

    for (int y=0; y<srcImg.H(); ++y) {
        const RGBX8 *src = srcImg.PtrConst_RGBX8(0,y);
        RGBA8 *dest = destImg->Ptr_RGBA8(0,y);
        for (int x=0; x<srcImg.W(); ++x) {
            *dest++ = *src++;
        }
    }
    return destImg;
}



Img* ConvertI8toI8(Img const& srcImg, Palette const& srcPalette, Palette const& destPalette) {
    assert(srcImg.Fmt() == FMT_I8);
    Img* destImg = new Img(FMT_I8, srcImg.W(), srcImg.H());

    for (int y=0; y<srcImg.H(); ++y) {
        const I8 *src = srcImg.PtrConst_I8(0,y);
        I8 *dest = destImg->Ptr_I8(0,y);
        for (int x=0; x<srcImg.W(); ++x) {
            Colour c = srcPalette.GetColour((int)*src++);
            *dest++ = (I8)destPalette.Closest(c);
        }
    }
    return destImg;
}


void RemapI8(Img& img, Palette const& srcPalette, Palette const& destPalette)
{
    assert(img.Fmt() == FMT_I8);
    for (int y = 0; y < img.H(); ++y) {
        I8* p = img.Ptr_I8(0, y);
        for (int x = 0; x < img.W(); ++x) {
            Colour c = srcPalette.GetColour((int)*p);
            *p = (I8)destPalette.Closest(c);
            ++p;
        }
    }
}

void RemapRGBX8(Img& img, Palette const& destPalette)
{
    assert(img.Fmt() == FMT_RGBX8);
    for (int y = 0; y < img.H(); ++y) {
        RGBX8 *p = img.Ptr_RGBX8(0, y);
        for (int x=0; x < img.W(); ++x) {
            I8 best = (I8)destPalette.Closest(Colour(*p));
            *p = destPalette.GetColour((int)best);
            ++p;
        }
    }
}


void RemapRGBA8(Img& img, Palette const& destPalette)
{
    assert(img.Fmt() == FMT_RGBA8);
    for (int y = 0; y < img.H(); ++y) {
        RGBA8 *p = img.Ptr_RGBA8(0, y);
        for (int x=0; x < img.W(); ++x) {
            I8 best = (I8)destPalette.Closest(Colour(*p));
            *p = destPalette.GetColour((int)best);
            ++p;
        }
    }
}

