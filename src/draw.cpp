#include "draw.h"
#include "img.h"
#include "palette.h"

#include <algorithm>    // for min,max
#include <vector>

static void FloodFill_I8( Img& img, Point const& start, I8 newcolour, Box& damage );
static void FloodFill_RGBX8( Img& img, Point const& start, RGBX8 newcolour, Box& damage );
static void FloodFill_RGBA8( Img& img, Point const& start, RGBA8 newcolour, Box& damage );

//-----------------------
// BLITTING FUNCTIONS
//-----------------------


// clips a blit against the destination boundary.
// assumes srcbox is already valid.
// modifies srcbox and destbox appropriately.
static void clip_blit(
    Box const& srcbounds, Box& srcbox,
    Box const& destbounds, Box& destbox,
    int xzoom=1,
    int yzoom=1 )
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


// blit an RGBA img, blending with src alpha onto the dest
void BlitRGBA8(
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
        switch(srcimg.Fmt())
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
            BlitMatteRGBX8Keyed(srcimg,srcbox,destimg,destbox,transparentcolour.rgb(), mattecolour.rgb());
            return;
        case FMT_RGBA8: 
            // TODO: do we really want this?
            BlitMatteRGBA8(srcimg,srcbox,destimg,destbox, mattecolour.rgb());
            return;
        default:
            assert(false);
    }
}


// Matte



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

static void scan_matte_I8_RGBA8_keyed(I8 const* src, RGBA8* dest, int w, I8 transparent, RGBX8 matte )
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

// blit src upon dest, blending with src.alpha
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

// blit src upon dest, blending with src.alpha
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
    RGBA8 matte )
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
        switch(srcimg.Fmt())
        {
            case FMT_I8:
                assert(false);  // not implemented
                break;
            case FMT_RGBX8:
                scan_matte_RGBX8_RGBX8_keyed(src, destimg.Ptr_RGBX8(destclipped.x+0,destclipped.y+y), w, transparent, RGBX8(matte.r,matte.g,matte.b));
                break;
            case FMT_RGBA8:
                scan_matte_RGBX8_RGBA8_keyed(src, destimg.Ptr_RGBA8(destclipped.x+0,destclipped.y+y), w, transparent, matte);
                break;
            default:
                assert(false);
                break;
        }
    }
    destbox = destclipped;
}

// blit src upon dest, blending with src.alpha
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

// blit src upon dest, blending with src.alpha
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
    RGBA8 matte )
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
        switch(srcimg.Fmt())
        {
            case FMT_I8:
                assert(false);  // not implemented
                break;
            case FMT_RGBX8:
                scan_matte_RGBA8_RGBX8(src, destimg.Ptr_RGBX8(destclipped.x+0,destclipped.y+y), w, matte);
                break;
            case FMT_RGBA8:
                scan_matte_RGBA8_RGBA8(src, destimg.Ptr_RGBA8(destclipped.x+0,destclipped.y+y), w, matte);
                break;
            default:
                assert(false);
                break;
        }
    }
    destbox = destclipped;
}



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

static void scan_zoom_alpha_RGBA8_RGBX8(RGBA8 const* src, RGBX8* dest, int w, int xzoom)
{
    int n=0;
    int x;
    for( x=0; x<w; ++x )
    {
        RGBX8 out = Blend(*src, *dest);
        *dest++ = out;
        if( ++n >= xzoom )  // on to next src pixel?
        {
            ++src;
            n=0;
        }
    }
}

void BlitZoomTransparent(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    Palette const& palette,
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
                palette,
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
            scan_zoom_alpha_RGBA8_RGBX8(
                srcimg.PtrConst_RGBA8(srcclipped.XMin()+0, srcclipped.YMin()+y/yzoom),
                    dest,
                    destclipped.W(),
                    xzoom );
            break;
        }
    }
}



static void scan_matte_zoom_alpha_RGBA8_RGBX8(RGBA8 const* src, RGBX8* dest, int w, RGBA8 matte, int xzoom)
{
    int n=0;
    int x;
    for( x=0; x<w; ++x )
    {
        RGBA8 in = *src;
        in.r = matte.r;
        in.g = matte.g;
        in.b = matte.b;
        RGBX8 out = Blend(in, *dest);
        *dest++ = out;
        if( ++n >= xzoom )  // on to next src pixel?
        {
            ++src;
            n=0;
        }
    }
}


void BlitZoomMatte(
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
        int x;
        RGBX8* dest = destimg.Ptr_RGBX8( destclipped.XMin() + 0, destclipped.YMin() + y );
        switch(srcimg.Fmt())
        {
        case FMT_I8:
            {
                // TODO: move into scan fn
                I8 const* src = srcimg.PtrConst_I8( srcclipped.XMin()+0, srcclipped.YMin()+y/yzoom );
                int n=0;
                for( x=0; x<destclipped.W(); ++x )
                {
                    if( *src != transparentcolour.idx())
                        *dest = mattecolour.rgb();
                    ++dest;
                    if( ++n >= xzoom )
                    {
                        ++src;
                        n=0;
                    }
                }
            }
            break;
        case FMT_RGBX8:
            {
                // TODO: move into scan fn
                RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.XMin()+0, srcclipped.YMin()+y/yzoom );
                int n=0;
                for( x=0; x<destclipped.W(); ++x )
                {
                    if( *src != transparentcolour.rgb())
                        *dest = mattecolour.rgb();
                    ++dest;
                    if( ++n >= xzoom )
                    {
                        ++src;
                        n=0;
                    }
                }
            }
            break;
        default:
            scan_matte_zoom_alpha_RGBA8_RGBX8(
                srcimg.PtrConst_RGBA8(srcclipped.XMin()+0, srcclipped.YMin()+y/yzoom),
                dest,
                destclipped.W(),
                mattecolour.rgb(),
                xzoom );
            break;
        }
    }
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



//--------------------
//


void FloodFill( Img& img, Point const& start, PenColour const& newcolour, Box& damage )
{
    switch(img.Fmt())
    {
        case FMT_I8:
            FloodFill_I8(img,start,newcolour.idx(),damage);
            break;
        case FMT_RGBX8:
            FloodFill_RGBX8(img,start,newcolour.rgb(),damage);
            break;
        case FMT_RGBA8:
            FloodFill_RGBA8(img,start,newcolour.rgb(),damage);
            break;
        default:
            assert(false);
            break;
    }
}


static void FloodFill_I8( Img& img, Point const& start, I8 newcolour, Box& damage )
{
    assert(img.Fmt()==FMT_I8);

    damage.SetEmpty();
    I8 oldcolour = img.Get_I8(start);
    if( oldcolour == newcolour )
        return;

    std::vector< Point > q;
    q.push_back( start );
    while( !q.empty() )
    {
        Point pt = q.back();
        q.pop_back();
        if( img.Get_I8(pt) != oldcolour )
            continue;

        // scan left and right to find span
        int y = pt.y;
        int l = pt.x;
        while( l>0 && img.Get_I8( Point(l-1,y)) == oldcolour )
            --l;
        int r = pt.x;
        while( r<img.W()-1 && img.Get_I8( Point(r+1,y)) == oldcolour )
            ++r;

        // fill the span
        I8* dest = img.Ptr_I8( l,y );
        int x;
        for( x=l; x<=r; ++x )
            *dest++ = newcolour;
        // expand the damage box to include the affected span
        damage.Merge( Box( l,y, (r+1)-l,1) );
        // add pixels above the span to the queue 
        y = pt.y-1;
        if( y>=0 )
        {
            for(x=l; x<=r; ++x)
            {
                if( img.Get_I8(Point(x,y)) == oldcolour )
                    q.push_back( Point(x,y) );
            }
        }

        // add pixels below the span to the queue
        y = pt.y+1;
        if( y<img.H() )
        {
            for(x=l; x<=r; ++x)
            {
                if( img.Get_I8(Point(x,y)) == oldcolour )
                    q.push_back( Point(x,y) );
            }
        }
    }
}



static void FloodFill_RGBX8( Img& img, Point const& start, RGBX8 newcolour, Box& damage )
{
    assert(img.Fmt()==FMT_RGBX8);

    damage.SetEmpty();
    RGBX8 oldcolour = img.Get_RGBX8(start);
    if( oldcolour == newcolour )
        return;

    std::vector< Point > q;
    q.push_back( start );
    while( !q.empty() )
    {
        Point pt = q.back();
        q.pop_back();
        if( img.Get_RGBX8(pt) != oldcolour )
            continue;

        // scan left and right to find span
        int y = pt.y;
        int l = pt.x;
        while( l>0 && img.Get_RGBX8( Point(l-1,y)) == oldcolour )
            --l;
        int r = pt.x;
        while( r<img.W()-1 && img.Get_RGBX8( Point(r+1,y)) == oldcolour )
            ++r;

        // fill the span
        RGBX8* dest = img.Ptr_RGBX8( l,y );
        int x;
        for( x=l; x<=r; ++x )
            *dest++ = newcolour;
        // expand the damage box to include the affected span
        damage.Merge( Box( l,y, (r+1)-l,1) );
        // add pixels above the span to the queue 
        y = pt.y-1;
        if( y>=0 )
        {
            for(x=l; x<=r; ++x)
            {
                if( img.Get_RGBX8(Point(x,y)) == oldcolour )
                    q.push_back( Point(x,y) );
            }
        }

        // add pixels below the span to the queue
        y = pt.y+1;
        if( y<img.H() )
        {
            for(x=l; x<=r; ++x)
            {
                if( img.Get_RGBX8(Point(x,y)) == oldcolour )
                    q.push_back( Point(x,y) );
            }
        }
    }
}

// TODO: should fill across differing alpha value?
static void FloodFill_RGBA8( Img& img, Point const& start, RGBA8 newcolour, Box& damage )
{
    assert(img.Fmt()==FMT_RGBA8);

    damage.SetEmpty();
    RGBA8 oldcolour = img.Get_RGBA8(start);
    if( oldcolour == newcolour )
        return;

    std::vector< Point > q;
    q.push_back( start );
    while( !q.empty() )
    {
        Point pt = q.back();
        q.pop_back();
        if( img.Get_RGBA8(pt) != oldcolour )
            continue;

        // scan left and right to find span
        int y = pt.y;
        int l = pt.x;
        while( l>0 && img.Get_RGBA8( Point(l-1,y)) == oldcolour )
            --l;
        int r = pt.x;
        while( r<img.W()-1 && img.Get_RGBA8( Point(r+1,y)) == oldcolour )
            ++r;

        // fill the span
        RGBA8* dest = img.Ptr_RGBA8( l,y );
        int x;
        for( x=l; x<=r; ++x )
            *dest++ = newcolour;
        // expand the damage box to include the affected span
        damage.Merge( Box( l,y, (r+1)-l,1) );
        // add pixels above the span to the queue 
        y = pt.y-1;
        if( y>=0 )
        {
            for(x=l; x<=r; ++x)
            {
                if( img.Get_RGBA8(Point(x,y)) == oldcolour )
                    q.push_back( Point(x,y) );
            }
        }

        // add pixels below the span to the queue
        y = pt.y+1;
        if( y<img.H() )
        {
            for(x=l; x<=r; ++x)
            {
                if( img.Get_RGBA8(Point(x,y)) == oldcolour )
                    q.push_back( Point(x,y) );
            }
        }
    }
}






// Bresenham line
void WalkLine(int x0, int y0, int x1, int y1, void (*plot)(int x, int y, void* user ), void* userdata )
{
    int dy = y1-y0;
    int dx = x1-x0;
    int xinc;
    int yinc;

    if( dx<0 )
        { xinc=-1; dx=-dx; }
    else
        { xinc=1; }
    dx*=2;

    if( dy<0 )
        { dy=-dy; yinc=-1; }
    else
        { yinc = 1; }
    dy*=2;

    plot( x0, y0, userdata );
    if( dx>dy )
    {
        // step along x axis
        int f = dy-(dx/2);
        while (x0 != x1)
        {
            if( f>=0 )
            {
                y0 += yinc;
                f -= dx;
            }
            f += dy;
            x0 += xinc;
            plot( x0, y0, userdata );
        }
    }
    else
    {
        // step along y axis
        int f = dx-(dy/2);
        while( y0 != y1 )
        {
            if (f >= 0)
            {
                x0 += xinc;
                f -= dy;
            }
            f += dx;
            y0 += yinc;
            plot( x0, y0, userdata );
        }
    }
}


// Bresenham ellipse
//
// Adapted from:
// http://willperone.net/Code/ellipse.php
//
void WalkEllipse(int xc, int yc, int r1, int r2,
    void (*drawpixel)(int x, int y, void* user ), void* userdata )
{
	int x= 0, y= r2, 
		a2= r1*r1, b2= r2*r2, 
		S, T;

	S = a2*(1-2*r2) + 2*b2;
	T = b2 - 2*a2*(2*r2-1);
	drawpixel(xc-x, yc-y, userdata);
	drawpixel(xc+x, yc+y, userdata);
	drawpixel(xc-x, yc+y, userdata);
	drawpixel(xc+x, yc-y, userdata);	
	do {
		if (S < 0)
		{
			S += 2*b2*(2*x + 3);
			T += 4*b2*(x + 1);
			x++;
		} else 
		if (T < 0)
		{
			S += 2*b2*(2*x + 3) - 4*a2*(y - 1);
			T += 4*b2*(x + 1) - 2*a2*(2*y - 3);
			x++;
			y--;
		} else {
			S -= 4*a2*(y - 1);
			T -= 2*a2*(2*y - 3);
			y--;
		}
		drawpixel(xc-x, yc-y, userdata);
		drawpixel(xc+x, yc+y, userdata);
		drawpixel(xc-x, yc+y, userdata);
		drawpixel(xc+x, yc-y, userdata);
	} while (y > 0);
}

// Bresenham ellipse
void WalkFilledEllipse(int xc, int yc, int r1, int r2,
    void (*drawhline)(int x0, int x1, int y, void* user ), void* userdata )
{
	int x= 0, y= r2, 
		a2= r1*r1, b2= r2*r2, 
		S, T;

	S = a2*(1-2*r2) + 2*b2;
	T = b2 - 2*a2*(2*r2-1);
    drawhline( xc-x, xc+x, yc-y, userdata );
    drawhline( xc-x, xc+x, yc+y, userdata );
	do {
		if (S < 0)
		{
			S += 2*b2*(2*x + 3);
			T += 4*b2*(x + 1);
			x++;
		} else 
		if (T < 0)
		{
			S += 2*b2*(2*x + 3) - 4*a2*(y - 1);
			T += 4*b2*(x + 1) - 2*a2*(2*y - 3);
			x++;
			y--;
		} else {
			S -= 4*a2*(y - 1);
			T -= 2*a2*(2*y - 3);
			y--;
		}
        drawhline( xc-x, xc+x, yc-y, userdata );
        drawhline( xc-x, xc+x, yc+y, userdata );
	} while (y > 0);
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
            return;
        case FMT_RGBA8:
            BlitRGBA8(srcimg, srcbox, destimg, destbox);
            return;
        default:
            assert(false);
    }
}

