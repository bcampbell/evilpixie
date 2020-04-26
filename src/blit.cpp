#include "blit.h"

#include "img.h"
#include "palette.h"

#include <algorithm>
#include <vector>

// clips a blit against the destination boundary.
// assumes srcbox is already valid.
// modifies srcbox and destbox appropriately.
void clip_blit(
    Box const& /* srcbounds */, Box& srcbox,
    Box const& destbounds, Box& destbox,
    int xzoom,
    int yzoom )
{
    // srcbox has blit dimensions
    destbox.w = srcbox.w*xzoom;
    destbox.h = srcbox.h*yzoom;

    // clip destbox, then adjust srcbox to take into account any
    // modifications
    int destx = destbox.x;
    int desty = destbox.y;
    destbox.ClipAgainst( destbounds );

    srcbox.x += (destbox.x - destx)/xzoom;
    srcbox.y += (destbox.y - desty)/yzoom;
    srcbox.w = destbox.w/xzoom;
    srcbox.h = destbox.h/yzoom;
}



void Blit(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox)
{
    assert( srcimg.Fmt() == destimg.Fmt());

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped );


    int y;
    for( y=0; y<destclipped.h; ++y )
    {
        switch(srcimg.Fmt())
        {
        case FMT_I8:
            {
                I8 const* src = srcimg.PtrConst_I8( srcclipped.x+0, srcclipped.y+y );
                I8* dest = destimg.Ptr_I8( destclipped.x+0, destclipped.y+y );
                std::copy( src,src+destclipped.w, dest);
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.x+0, srcclipped.y+y );
                RGBX8* dest = destimg.Ptr_RGBX8( destclipped.x+0, destclipped.y+y );
                std::copy( src,src+destclipped.w, dest);
            }
            break;
        case FMT_RGBA8:
            {
                RGBA8 const* src = srcimg.PtrConst_RGBA8( srcclipped.x+0, srcclipped.y+y );
                RGBA8* dest = destimg.Ptr_RGBA8( destclipped.x+0, destclipped.y+y );
                std::copy( src,src+destclipped.w, dest);
            }
            break;
        default:
            assert(false);
            break;
        }
    }

    destbox = destclipped;
}


// TODO: src,dest names meaningless. Should be a,b or something neutral
void BlitSwap(
    Img& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox)
{
    assert( srcimg.Fmt() == destimg.Fmt());

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped );


    int y;
    for( y=0; y<destclipped.h; ++y )
    {
        switch(srcimg.Fmt())
        {
        case FMT_I8:
            {
                I8* src = srcimg.Ptr_I8( srcclipped.x+0, srcclipped.y+y );
                I8* dest = destimg.Ptr_I8( destclipped.x+0, destclipped.y+y );
                std::swap_ranges( src,src+destclipped.w, dest);
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8* src = srcimg.Ptr_RGBX8( srcclipped.x+0, srcclipped.y+y );
                RGBX8* dest = destimg.Ptr_RGBX8( destclipped.x+0, destclipped.y+y );
                std::swap_ranges( src,src+destclipped.w, dest);
            }
            break;
        case FMT_RGBA8:
            {
                RGBA8* src = srcimg.Ptr_RGBA8( srcclipped.x+0, srcclipped.y+y );
                RGBA8* dest = destimg.Ptr_RGBA8( destclipped.x+0, destclipped.y+y );
                std::swap_ranges( src,src+destclipped.w, dest);
            }
            break;
        default:
            assert(false);
            break;
        }
    }

    destbox = destclipped;
}


#if 0

// blit src upon dest, blending with src.alpha
static void scan_RGBA8_RGBX8(RGBA8 const* src, RGBX8* dest, int w)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBX8 out = Blend(*src++, *dest);
        *dest++ = out;
    }
}

// blit src upon dest, blending with src.alpha
static void scan_RGBA8_RGBA8(RGBA8 const* src, RGBA8* dest, int w)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 out = Blend(*src++, *dest);
        *dest++ = out;
    }
}


// blit an RGBA img, blending with src alpha onto the dest (must be RGBX8 or RGBA8)
void BlitRGBA8( Img const& srcimg, Box const& srcbox, Img& destimg, Box& destbox )
{
    assert(srcimg.Fmt()==FMT_RGBA8);
    assert(destimg.Fmt()!=FMT_I8);

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
                assert(false);  // not implemented
                break;
            case FMT_RGBX8:
                scan_RGBA8_RGBX8(src, destimg.Ptr_RGBX8(destclipped.x+0,destclipped.y+y), w);
                break;
            case FMT_RGBA8:
                scan_RGBA8_RGBA8(src, destimg.Ptr_RGBA8(destclipped.x+0,destclipped.y+y), w);
                break;
            default:
                assert(false);
                break;
        }
    }
    destbox = destclipped;
}

#endif


//----------------------------------------------



//-----------------------------------------


#if 0

static void scan_matte_RGBA8_I8(RGBA8 const* src, I8* dest, int w, I8 matte)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 c = *src++;
        if (c.a != 0) {
            *dest = matte;
        }
        ++dest;
    }
}


static void scan_matte_RGBA8_RGBX8(RGBA8 const* src, RGBX8* dest, int w, RGBA8 matte)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 in = *src++;
        in.r = matte.r;
        in.g = matte.g;
        in.b = matte.b;
        RGBX8 out = Blend(in, *dest);
        *dest++ = out;
    }
}

static void scan_matte_RGBA8_RGBA8(RGBA8 const* src, RGBA8* dest, int w, RGBA8 matte)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 in = *src++;
        in.r = matte.r;
        in.g = matte.g;
        in.b = matte.b;
        RGBA8 out = Blend(in, *dest);
        *dest++ = out;
    }
}


// blit an RGBA img, blending matte colour with src alpha onto the dest
void BlitMatteRGBA8(
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
                scan_matte_RGBA8_I8(src, destimg.Ptr_I8(destclipped.x+0,destclipped.y+y), w, matte.idx());
                break;
            case FMT_RGBX8:
                scan_matte_RGBA8_RGBX8(src, destimg.Ptr_RGBX8(destclipped.x+0,destclipped.y+y), w, matte.rgb());
                break;
            case FMT_RGBA8:
                scan_matte_RGBA8_RGBA8(src, destimg.Ptr_RGBA8(destclipped.x+0,destclipped.y+y), w, matte.rgb());
                break;
            default:
                assert(false);
                break;
        }
    }
    destbox = destclipped;
}

#endif

