#include "blit_keyed.h"
#include "blit.h"
#include "img.h"
#include "palette.h"

// Keyed blits - blit I8 to dest, with a single transparent colour.

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


static void scan_RGBA8_I8_keyed(RGBA8 const* src, I8* dest, int w )
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 c = *src++;
        if( c.a > 0)
            *dest = 1;      // TODO:!!!!
        ++dest;
    }
}

static void scan_RGBA8_RGBX8_keyed(RGBA8 const* src, RGBX8* dest, int w ) 
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 c = *src++;
        if( c.a > 0)
            *dest = RGBX8(c.r, c.g, c.b);
        ++dest;
    }
}

static void scan_RGBA8_RGBA8_keyed(RGBA8 const* src, RGBA8* dest, int w )
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 c = *src++;
        if( c.a > 0)
            *dest = c;
        ++dest;
    }
}




// blit from an RGBX8 source to any target, with colourkey transparency
void BlitRGBA8Keyed(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox )
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
            scan_RGBA8_I8_keyed(src, destimg.Ptr_I8(destclipped.x+0,destclipped.y+y), w );
            break;
        case FMT_RGBX8:
            scan_RGBA8_RGBX8_keyed(src, destimg.Ptr_RGBX8(destclipped.x+0,destclipped.y+y), w );
            break;
        case FMT_RGBA8:
            scan_RGBA8_RGBA8_keyed(src, destimg.Ptr_RGBA8(destclipped.x+0,destclipped.y+y), w );
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

// TODO: - temporary?
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
                BlitRGBA8Keyed(srcimg, srcbox, destimg, destbox);
            return;
        default:
            assert(false);
    }
}

