#include "draw.h"
#include "img.h"
#include "palette.h"

#include <algorithm>    // for min,max

//-----------------------
// BLITTING FUNCTIONS
//-----------------------


// clips a blit against the destination boundary.
// assumes srcbox is already valid.
// modifies srcbox and destbox appropriately.
void clip_blit(
    Box const& srcbounds, Box& srcbox,
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





static void scan_I8_I8_keyed(I8 const* src, I8* dest, int w, I8 transparent)
{
    int x;
    for( x=0; x<w; ++x )
    {
        I8 c = *src++;
        if( c != transparent)
            *dest = c;
        ++dest;
    }
}

static void scan_I8_RGBX8_keyed(I8 const* src, Palette const& pal, RGBX8* dest, int w, I8 transparent)
{
    int x;
    for( x=0; x<w; ++x )
    {
        I8 c = *src++;
        if( c != transparent)
            *dest = pal.GetColour(c);
        ++dest;
    }
}

static void scan_I8_RGBA8_keyed(I8 const* src, Palette const& pal, RGBA8* dest, int w, I8 transparent)
{
    int x;
    for( x=0; x<w; ++x )
    {
        I8 c = *src++;
        if( c != transparent)
            *dest = pal.GetColour(c);
        ++dest;
    }
}




// blit from an I8 source to any target, with colourkey transparency
void BlitI8Keyed(
    Img const& srcimg, Box const& srcbox,
    Palette const& srcpalette,
    Img& destimg, Box& destbox,
    int transparentIdx )
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
            scan_I8_I8_keyed(src, destimg.Ptr_I8(destclipped.x+0,destclipped.y+y), w, transparentIdx);
            break;
        case FMT_RGBX8:
            scan_I8_RGBX8_keyed(src, srcpalette, destimg.Ptr_RGBX8(destclipped.x+0,destclipped.y+y), w, transparentIdx);
            break;
        case FMT_RGBA8:
            scan_I8_RGBA8_keyed(src, srcpalette, destimg.Ptr_RGBA8(destclipped.x+0,destclipped.y+y), w, transparentIdx);
            break;
        default:
            assert(false);
            break;
        }
    }

    destbox = destclipped;
}

//-------------------------------------------------------------------

static void scan_RGBX8_I8_keyed(RGBX8 const* src, I8* dest, int w, RGBX8 transparent)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBX8 c = *src++;
        if( c != transparent)
            *dest = 1;      // TODO:!!!!
        ++dest;
    }
}

static void scan_RGBX8_RGBX8_keyed(RGBX8 const* src, RGBX8* dest, int w, RGBX8 transparent)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBX8 c = *src++;
        if( c != transparent)
            *dest = c;
        ++dest;
    }
}

static void scan_RGBX8_RGBA8_keyed(RGBX8 const* src, RGBA8* dest, int w, RGBX8 transparent)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBX8 c = *src++;
        if( c != transparent)
            *dest = c;
        ++dest;
    }
}




// blit from an RGBX8 source to any target, with colourkey transparency
void BlitRGBX8Keyed(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    RGBX8 transparent )
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
            scan_RGBX8_I8_keyed(src, destimg.Ptr_I8(destclipped.x+0,destclipped.y+y), w, transparent);
            break;
        case FMT_RGBX8:
            scan_RGBX8_RGBX8_keyed(src, destimg.Ptr_RGBX8(destclipped.x+0,destclipped.y+y), w, transparent);
            break;
        case FMT_RGBA8:
            scan_RGBX8_RGBA8_keyed(src, destimg.Ptr_RGBA8(destclipped.x+0,destclipped.y+y), w, transparent);
            break;
        default:
            assert(false);
            break;
        }
    }

    destbox = destclipped;
}



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


//----------------------------------------------



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



//-----------------------------------------

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

//----------------------------------------


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
            BlitMatteRGBA8(srcimg,srcbox,destimg,destbox, mattecolour);
            return;
        default:
            assert(false);
    }
}

//----------------------------------------


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

//XYZZY - temporary?
void BlitTransparent(
    Img const& srcimg, Box const& srcbox,
    Palette const& srcpalette,
    Img& destimg, Box& destbox,
    PenColour const& transparentcolour )
{
    switch(srcimg.Fmt())
    {
        case FMT_I8:
            assert(transparentcolour.IdxValid());
            //assert(srcpalette!=0);
            BlitI8Keyed(srcimg,srcbox,srcpalette, destimg, destbox, transparentcolour.idx());
            return;
        case FMT_RGBX8:
            if (destimg.Fmt() != FMT_I8)
                BlitRGBX8Keyed(srcimg,srcbox, destimg, destbox, transparentcolour.rgb());
            return;
        case FMT_RGBA8:
            if (destimg.Fmt() != FMT_I8)
                BlitRGBA8(srcimg, srcbox, destimg, destbox);
            return;
        default:
            assert(false);
    }
}

