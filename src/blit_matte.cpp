#include "blit_matte.h"
#include "blit.h"
#include "img.h"
#include "palette.h"


static void scan_matte_I8_I8_keyed(I8 const* src, I8* dest, int w, I8 transparent, I8 matte)
{
    int x;
    for( x=0; x<w; ++x )
    {
        I8 c = *src++;
        if( c != transparent)
            *dest = matte;
        ++dest;
    }
}

static void scan_matte_I8_RGBX8_keyed(I8 const* src, RGBX8* dest, int w, I8 transparent, RGBX8 matte)
{
    int x;
    for( x=0; x<w; ++x )
    {
        I8 c = *src++;
        if( c != transparent)
            *dest = matte;
        ++dest;
    }
}

static void scan_matte_I8_RGBA8_keyed(I8 const* src, RGBA8* dest, int w, I8 transparent, RGBA8 matte )
{
    int x;
    for( x=0; x<w; ++x )
    {
        I8 c = *src++;
        if( c != transparent)
            *dest = matte;
        ++dest;
    }
}




// blit from an I8 source to any target, with colourkey transparency
void BlitMatteI8Keyed(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    int transparentIdx,
    PenColour const& mattecolour )
{
    assert(srcimg.Fmt()==FMT_I8);

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped );

    const int w = destclipped.w;

    int y;
    for( y=0; y<destclipped.h; ++y )
    {
        I8 const* src = srcimg.PtrConst_I8( srcclipped.x+0, srcclipped.y+y );
        switch(destimg.Fmt())
        {
        case FMT_I8:
            scan_matte_I8_I8_keyed(src, destimg.Ptr_I8(destclipped.x+0,destclipped.y+y), w, transparentIdx, mattecolour.idx());
            break;
        case FMT_RGBX8:
            scan_matte_I8_RGBX8_keyed(src, destimg.Ptr_RGBX8(destclipped.x+0,destclipped.y+y), w, transparentIdx, mattecolour.rgb());
            break;
        case FMT_RGBA8:
            scan_matte_I8_RGBA8_keyed(src, destimg.Ptr_RGBA8(destclipped.x+0,destclipped.y+y), w, transparentIdx, mattecolour.rgb());
            break;
        default:
            assert(false);
            break;
        }
    }

    destbox = destclipped;
}


//-----------------------------------------------------------

static void scan_matte_RGBX8_I8_keyed(RGBX8 const* src, I8* dest, int w, RGBX8 transparent, I8 matte)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBX8 in = *src++;
        if(in!=transparent) {
            *dest = matte;
        }
        ++dest;
    }
}


static void scan_matte_RGBX8_RGBX8_keyed(RGBX8 const* src, RGBX8* dest, int w, RGBX8 transparent, RGBX8 matte)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBX8 in = *src++;
        if(in!=transparent) {
            *dest = matte;
        }
        ++dest;
    }
}

static void scan_matte_RGBX8_RGBA8_keyed(RGBX8 const* src, RGBA8* dest, int w, RGBX8 transparent, RGBA8 matte)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBX8 in = *src++;
        if(in!=transparent) {
            *dest = matte;
        }
        ++dest;
    }
}


// blit an RGBX img
void BlitMatteRGBX8Keyed(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    RGBX8 transparent,
    PenColour const& matte )
{
    assert(srcimg.Fmt()==FMT_RGBX8);

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped );

    const int w = destclipped.w;
    int y;
    for( y=0; y<destclipped.h; ++y )
    {
        RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.x+0, srcclipped.y+y );
        switch(destimg.Fmt())
        {
            case FMT_I8:
                scan_matte_RGBX8_I8_keyed(src, destimg.Ptr_I8(destclipped.x+0,destclipped.y+y), w, transparent, matte.idx());
                break;
            case FMT_RGBX8:
                scan_matte_RGBX8_RGBX8_keyed(src, destimg.Ptr_RGBX8(destclipped.x+0,destclipped.y+y), w, transparent, matte.rgb());
                break;
            case FMT_RGBA8:
                scan_matte_RGBX8_RGBA8_keyed(src, destimg.Ptr_RGBA8(destclipped.x+0,destclipped.y+y), w, transparent, matte.rgb());
                break;
            default:
                assert(false);
                break;
        }
    }
    destbox = destclipped;
}

//-----------------------------------------------------------

static void scan_matte_RGBA8_I8_keyed(RGBA8 const* src, I8* dest, int w, I8 matte)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 in = *src++;
        if(in.a>0) {
            *dest = matte;
        }
        ++dest;
    }
}


static void scan_matte_RGBA8_RGBX8_keyed(RGBA8 const* src, RGBX8* dest, int w, RGBX8 matte)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 in = *src++;
        if(in.a>0) {
            *dest = matte;
        }
        ++dest;
    }
}

static void scan_matte_RGBA8_RGBA8_keyed(RGBA8 const* src, RGBA8* dest, int w, RGBA8 matte)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 in = *src++;
        if(in.a>0) {
            *dest = matte;
        }
        ++dest;
    }
}


// blit an RGBA img
void BlitMatteRGBA8Keyed(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    PenColour const& matte )
{
    assert(srcimg.Fmt()==FMT_RGBA8);

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped );

    const int w = destclipped.w;
    int y;
    for( y=0; y<destclipped.h; ++y )
    {
        RGBA8 const* src = srcimg.PtrConst_RGBA8( srcclipped.x+0, srcclipped.y+y );
        switch(destimg.Fmt())
        {
            case FMT_I8:
                scan_matte_RGBA8_I8_keyed(src, destimg.Ptr_I8(destclipped.x+0,destclipped.y+y), w, matte.idx());
                break;
            case FMT_RGBX8:
                scan_matte_RGBA8_RGBX8_keyed(src, destimg.Ptr_RGBX8(destclipped.x+0,destclipped.y+y), w, matte.rgb());
                break;
            case FMT_RGBA8:
                scan_matte_RGBA8_RGBA8_keyed(src, destimg.Ptr_RGBA8(destclipped.x+0,destclipped.y+y), w, matte.rgb());
                break;
            default:
                assert(false);
                break;
        }
    }
    destbox = destclipped;
}


// blit the src as a matte
void BlitMatte(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    PenColour const& transparentcolour,
    PenColour const& mattecolour )
{
    switch(srcimg.Fmt())
    {
        case FMT_I8:
            BlitMatteI8Keyed(srcimg,srcbox,destimg,destbox,transparentcolour.idx(), mattecolour);
            return;
        case FMT_RGBX8:
            BlitMatteRGBX8Keyed(srcimg,srcbox,destimg,destbox,transparentcolour.rgb(), mattecolour);
            return;
        case FMT_RGBA8: 
            BlitMatteRGBA8Keyed(srcimg,srcbox,destimg,destbox, mattecolour);
            return;
        default:
            assert(false);
    }
}

