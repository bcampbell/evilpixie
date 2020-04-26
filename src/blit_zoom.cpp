#include "blit_zoom.h"
#include "blit.h"
#include "img.h"
#include "palette.h"


// TODO: should probably kill most of these. Only needed because there's no
// real integration between tools and view rendering.


static void scan_zoom_keyed_I8_RGBX8(I8 const* src, Palette const& pal, RGBX8* dest, int w, I8 transparent, int xzoom)
{
    int n=0;
    int x;
    for( x=0; x<w; ++x )
    {
        if(*src != transparent)
        {
            RGBX8 c = pal.GetColour(*src);
            *dest = c;
        }
        ++dest;
        if( ++n >= xzoom )
        {
            ++src;
            n=0;
        }
    }
}

static void scan_zoom_keyed_RGBX8_RGBX8(RGBX8 const* src, RGBX8* dest, int w, RGBX8 transparent, int xzoom)
{
    int n=0;
    int x;
    for( x=0; x<w; ++x )
    {
        RGBX8 c=*src;
        if( c != transparent)
            *dest = c;
        ++dest;
        if( ++n >= xzoom )
        {
            ++src;
            n=0;
        }
    }
}

static void scan_zoom_keyed_RGBA8_RGBX8(RGBA8 const* src, RGBX8* dest, int w, int xzoom)
{
    int n=0;
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 c = *src;
        if( c.a>0 )
            *dest = RGBX8(c.r, c.g, c.b);
        ++dest;
        if( ++n >= xzoom )  // on to next src pixel?
        {
            ++src;
            n=0;
        }
    }
}

void BlitZoomKeyed(
    Img const& srcimg, Box const& srcbox,
    Palette const& srcpalette,
    Img& destimg, Box& destbox,
    int xzoom,
    int yzoom,
    PenColour const& transparentcolour)
{
    assert( destimg.Fmt()==FMT_RGBX8);
    assert( srcimg.Bounds().Contains( srcbox ) );
    assert( xzoom >= 1 );
    assert( yzoom >= 1 );

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped, xzoom, yzoom );

    int y;
    for( y=0; y<destclipped.H(); ++y )
    {
        RGBX8* dest = destimg.Ptr_RGBX8( destclipped.XMin() + 0, destclipped.YMin() + y );
        switch(srcimg.Fmt())
        {
        case FMT_I8:
            scan_zoom_keyed_I8_RGBX8(
                srcimg.PtrConst_I8( srcclipped.XMin()+0, srcclipped.YMin()+y/yzoom ),
                srcpalette,
                dest,
                destclipped.W(),
                transparentcolour.idx(),
                xzoom );
            break;
        case FMT_RGBX8:
            scan_zoom_keyed_RGBX8_RGBX8(
                srcimg.PtrConst_RGBX8( srcclipped.XMin()+0, srcclipped.YMin()+y/yzoom ),
                    dest,
                    destclipped.W(),
                    transparentcolour.rgb(),
                    xzoom );
            break;
        default:
            scan_zoom_keyed_RGBA8_RGBX8(
                srcimg.PtrConst_RGBA8(srcclipped.XMin()+0, srcclipped.YMin()+y/yzoom),
                    dest,
                    destclipped.W(),
                    xzoom );
            break;
        }
    }
}



//-----------------------------------------------------
//

static void scan_zoom_matte_keyed_RGBA8_RGBX8(RGBA8 const* src, RGBX8* dest, int w, int xzoom, RGBX8 matte)
{
    int x=0;
    while(x<w)
    {
        RGBA8 c = *src++;
        if (c.a>0) {
            int n;
            for( n=0; n<xzoom && (x+n)<w; ++n )
                *dest++ = matte;
        } else {
            dest += xzoom;
        }
        x += xzoom;
    }
}

static void scan_zoom_matte_keyed_RGBX8_RGBX8(RGBX8 const* src, RGBX8* dest, int w, int xzoom, RGBX8 transparent, RGBX8 matte)
{
    int x=0;
    while(x<w)
    {
        RGBX8 c = *src++;
        if (c!=transparent) {
            int n;
            for( n=0; n<xzoom && (x+n)<w; ++n )
                *dest++ = matte;
        } else {
            dest += xzoom;
        }
        x += xzoom;
    }
}


static void scan_zoom_matte_keyed_I8_RGBX8(I8 const* src, RGBX8* dest, int w, int xzoom, I8 transparent, RGBX8 matte)
{
    int x=0;
    while(x<w)
    {
        I8 c = *src++;
        if (c!=transparent) {
            int n;
            for( n=0; n<xzoom && (x+n)<w; ++n )
                *dest++ = matte;
        } else {
            dest += xzoom;
        }
        x += xzoom;
    }
}




void BlitZoomMatteKeyed(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    int xzoom,
    int yzoom,
    PenColour const& transparentcolour,
    PenColour const& mattecolour )
{
    assert( destimg.Fmt()==FMT_RGBX8);
    assert( srcimg.Bounds().Contains( srcbox ) );
    assert( xzoom >= 1 );
    assert( yzoom >= 1 );

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped, xzoom,yzoom );

    int y;
    for( y=0; y<destclipped.H(); ++y )
    {
        RGBX8* dest = destimg.Ptr_RGBX8( destclipped.XMin() + 0, destclipped.YMin() + y );
        switch(srcimg.Fmt())
        {
        case FMT_I8:
            scan_zoom_matte_keyed_I8_RGBX8(
                srcimg.PtrConst_I8(srcclipped.XMin()+0, srcclipped.YMin()+y/yzoom),
                dest,
                destclipped.W(),
                xzoom,
                transparentcolour.idx(),
                mattecolour.rgb() );
            break;
        case FMT_RGBX8:
            scan_zoom_matte_keyed_RGBX8_RGBX8(
                srcimg.PtrConst_RGBX8(srcclipped.XMin()+0, srcclipped.YMin()+y/yzoom),
                dest,
                destclipped.W(),
                xzoom,
                transparentcolour.rgb(),
                mattecolour.rgb() );
            break;
        case FMT_RGBA8:
            scan_zoom_matte_keyed_RGBA8_RGBX8(
                srcimg.PtrConst_RGBA8(srcclipped.XMin()+0, srcclipped.YMin()+y/yzoom),
                dest,
                destclipped.W(),
                xzoom,
                mattecolour.rgb() );
            break;
        default:
            assert(false);
            break;
        }
    }
}



// ***********************************************

static void scan_zoom_I8_I8(I8 const* src, I8* dest, int w, int xzoom)
{
    int n=0;
    int x;
    for( x=0; x<w; ++x )
    {
        *dest++ = *src;
        if( ++n >= xzoom )
        {
            ++src;
            n=0;
        }
    }
}

static void scan_zoom_I8_RGBX8(I8 const* src, Palette const& pal, RGBX8* dest, int w, int xzoom)
{
    int n=0;
    int x;
    RGBX8 c = pal.GetColour(*src);
    for( x=0; x<w; ++x )
    {
        *dest++ = c;
        if( ++n >= xzoom )
        {
            ++src;
            c = pal.GetColour(*src);
            n=0;
        }
    }
}

static void scan_zoom_I8_RGBA8(I8 const* src, Palette const& pal, RGBA8* dest, int w, int xzoom)
{
    int n=0;
    int x;
    RGBA8 c = pal.GetColour(*src);
    for( x=0; x<w; ++x )
    {
        *dest++ = c;
        if( ++n >= xzoom )
        {
            ++src;
            c = pal.GetColour(*src);
            n=0;
        }
    }
}


void BlitZoomI8(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    Palette const& pal,
    int xzoom,
    int yzoom )
{
    assert( srcimg.Fmt()==FMT_I8);
    assert( srcimg.Bounds().Contains( srcbox ) );
    assert( xzoom >= 1 );
    assert( yzoom >= 1 );

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped, xzoom, yzoom );

    int y;
    for( y=0; y<destclipped.H(); ++y )
    {
        I8 const* src = srcimg.PtrConst_I8( srcclipped.XMin()+0, srcclipped.YMin()+y/yzoom );
        switch(destimg.Fmt())
        {
        case FMT_I8:
            scan_zoom_I8_I8(
                src,
                destimg.Ptr_I8( destclipped.XMin() + 0, destclipped.YMin() + y ),
                destclipped.W(),
                xzoom );
            break;
        case FMT_RGBX8:
            scan_zoom_I8_RGBX8(
                src,
                pal,
                destimg.Ptr_RGBX8( destclipped.XMin() + 0, destclipped.YMin() + y ),
                destclipped.W(),
                xzoom );
            break;
        case FMT_RGBA8:
            scan_zoom_I8_RGBA8(
                src,
                pal,
                destimg.Ptr_RGBA8( destclipped.XMin() + 0, destclipped.YMin() + y ),
                destclipped.W(),
                xzoom );
            break;
        default:
            assert(false);
            break;
        }
    }
}


// ***********************************************


static void scan_zoom_RGBX8_RGBX8(RGBX8 const* src, RGBX8* dest, int w, int xzoom)
{
    int n=0;
    int x;
    RGBX8 c = *src++;
    for( x=0; x<w; ++x )
    {
        *dest++ = c;
        if( ++n >= xzoom )
        {
            c = *src++;
            n=0;
        }
    }
}

static void scan_zoom_RGBX8_RGBA8(RGBX8 const* src, RGBA8* dest, int w, int xzoom)
{
    int n=0;
    int x;
    RGBX8 c = *src++;
    for( x=0; x<w; ++x )
    {
        *dest++ = c;
        if( ++n >= xzoom )
        {
            c = *src++;
            n=0;
        }
    }
}


void BlitZoomRGBX8(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    int xzoom,
    int yzoom )
{
    assert( srcimg.Fmt()==FMT_RGBX8);
    assert( srcimg.Bounds().Contains( srcbox ) );
    assert( xzoom >= 1 );
    assert( yzoom >= 1 );

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped, xzoom, yzoom );

    int y;
    for( y=0; y<destclipped.H(); ++y )
    {
        RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.XMin()+0, srcclipped.YMin()+y/yzoom );
        switch(destimg.Fmt())
        {
        case FMT_I8:
            assert(false);  // not supported
            break;
        case FMT_RGBX8:
            scan_zoom_RGBX8_RGBX8(
                src,
                destimg.Ptr_RGBX8( destclipped.XMin() + 0, destclipped.YMin() + y ),
                destclipped.W(),
                xzoom );
            break;
        case FMT_RGBA8:
            scan_zoom_RGBX8_RGBA8(
                src,
                destimg.Ptr_RGBA8( destclipped.XMin() + 0, destclipped.YMin() + y ),
                destclipped.W(),
                xzoom );
            break;
        default:
            assert(false);
            break;
        }
    }
}


// ***********************************************


static void scan_zoom_RGBA8_RGBX8(RGBA8 const* src, RGBX8* dest, int w, int xzoom)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 foo = *src++;
        RGBX8 c(foo.r, foo.g, foo.b);
        int n;
        for( n=0; n<xzoom && (x+n)<w; ++n )
            *dest++ = c;
    }
}

static void scan_zoom_RGBA8_RGBA8(RGBA8 const* src, RGBA8* dest, int w, int xzoom)
{
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 c = *src++;
        int n;
        for( n=0; n<xzoom && (x+n)<w; ++n )
            *dest++ = c;
    }
}


void BlitZoomRGBA8(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    int xzoom,
    int yzoom )
{
    assert( srcimg.Fmt()==FMT_RGBA8);
    assert( srcimg.Bounds().Contains( srcbox ) );
    assert( xzoom >= 1 );
    assert( yzoom >= 1 );

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped, xzoom, yzoom );

    int y;
    for( y=0; y<destclipped.H(); ++y )
    {
        RGBA8 const* src = srcimg.PtrConst_RGBA8( srcclipped.XMin()+0, srcclipped.YMin()+y/yzoom );
        switch(destimg.Fmt())
        {
        case FMT_I8:
            assert(false);  // not supported
            break;
        case FMT_RGBX8:
            scan_zoom_RGBA8_RGBX8(
                src,
                destimg.Ptr_RGBX8( destclipped.XMin() + 0, destclipped.YMin() + y ),
                destclipped.W(),
                xzoom );
            break;
        case FMT_RGBA8:
            scan_zoom_RGBA8_RGBA8(
                src,
                destimg.Ptr_RGBA8( destclipped.XMin() + 0, destclipped.YMin() + y ),
                destclipped.W(),
                xzoom );
            break;
        default:
            assert(false);
            break;
        }
    }
}


// ***********************************************
void BlitZoom(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    Palette const& palette,
    int xzoom,
    int yzoom )
{
    switch( srcimg.Fmt() )
    {
        case FMT_I8:
            BlitZoomI8(srcimg,srcbox,destimg,destbox,palette,xzoom,yzoom);
            break;
        case FMT_RGBX8:
            BlitZoomRGBX8(srcimg,srcbox,destimg,destbox,xzoom,yzoom);
            break;
        case FMT_RGBA8:
            BlitZoomRGBA8(srcimg,srcbox,destimg,destbox,xzoom,yzoom);
            break;
        default:
            assert(false);
            break;
    }
}

