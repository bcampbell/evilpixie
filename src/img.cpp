#include "img.h"
#include "palette.h"

#include <cstring>
#include <cstdio>
#include <cassert>
#include <algorithm>    // for reverse()

Img::Img( PixelFormat pixel_format, int w, int h, uint8_t const* initial ) :
    m_Format(pixel_format),
    m_BytesPerPixel(0),
    m_BytesPerRow(0),
    m_Bounds(0,0,w,h),
    m_Pixels(0)
{
    init();
    if(initial) {
        memcpy( m_Pixels, initial, H()*m_BytesPerRow );
    } else {
        memset( m_Pixels, 0, H()*m_BytesPerRow );
    }
}

Img::Img( Img const& other ) :
    m_Format(other.m_Format),
    m_BytesPerPixel(0),
    m_BytesPerRow(0),
    m_Bounds(other.m_Bounds),
    m_Pixels(0)
{
    init();
}
    
Img::Img( Img const& other, Box const& otherarea ) :
    m_Format(other.m_Format),
    m_BytesPerPixel(0),
    m_BytesPerRow(0),
    m_Bounds(0,0,otherarea.w,otherarea.h),
    m_Pixels(0)
{
    init();
    Box b(m_Bounds);
    Blit(other, otherarea, *this, b);
}


// set up stuff that depends on pixelformat 
void Img::init()
{
    assert(m_Bounds.x==0 && m_Bounds.y==0);

    switch(m_Format)
    {
        case FMT_I8: m_BytesPerPixel=1; break;
        case FMT_RGBX8: m_BytesPerPixel=4; break;
    }
    assert(m_BytesPerPixel>0);
    m_BytesPerRow = m_Bounds.w*m_BytesPerPixel;
    m_Pixels = new uint8_t[m_BytesPerRow*m_Bounds.h];
}


void Img::Copy( Img const& other )
{
    delete [] m_Pixels;
    m_Format = other.m_Format;
    m_Bounds = other.m_Bounds;
    init();
    Box b(m_Bounds);
    Blit(other, other.Bounds(), *this, b);
}


void Img::HLine( PenColour const& pen, int xbegin, int xend, int y)
{
    int x;
    switch(Fmt())
    {
        case FMT_I8:
            {
                assert(pen.IdxValid());
                I8* dest = Ptr_I8(xbegin,y);
                for( x=xbegin; x<xend; ++x )
                    *dest++ = pen.idx();
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8* dest = Ptr_RGBX8(xbegin,y);
                for( x=xbegin; x<xend; ++x )
                    *dest++ = pen.rgb();
            }
            break;
        default: assert(false); // not implemented
    }
}



void Img::FillBox( PenColour const& pen, Box& b )
{
    b.ClipAgainst( Bounds() );
    int y;
    for( y=b.YMin(); y<=b.YMax(); ++y )
        HLine(pen,b.XMin(),b.XMax()+1,y);
}


void Img::XFlip()
{
    assert(Fmt()==FMT_I8);
    int y;
    for(y=0; y<H(); ++y)
    {
        switch(Fmt())
        {
        case FMT_I8:
            {
                I8* begin = Ptr_I8(0,y);
                std::reverse(begin,begin+W());
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8* begin = Ptr_RGBX8(0,y);
                std::reverse(begin,begin+W());
            }
            break;
        default: assert(false); // not implemented
        }
    }
}

void Img::YFlip()
{
    assert(Fmt()==FMT_I8);
    int y;
    for(y=0; y<H()/2; ++y)
    {
        switch(Fmt())
        {
        case FMT_I8:
            {
                I8* a = Ptr_I8(0,y);
                I8* b = Ptr_I8(0,(H()-1)-y);
                std::swap_ranges(a,a+W(),b);
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8* a = Ptr_RGBX8(0,y);
                RGBX8* b = Ptr_RGBX8(0,(H()-1)-y);
                std::swap_ranges(a,a+W(),b);
            }
            break;
        default: assert(false); // not implemented
        }
    }
}



//-----------------------


void Img::OutlineBox( PenColour const& pen, Box& b )
{
    b.ClipAgainst( Bounds() );

    if( Fmt()==FMT_RGBX8)
    {
        HLine(pen, b.XMin(), b.XMax()+1, b.YMin());
        HLine(pen, b.XMin(), b.XMax()+1, b.YMax());

        // draw sides (note: already draw top & bottom pixels)
        RGBX8* pleft = Ptr_RGBX8(b.XMin(),b.YMin()+1);
        RGBX8* pright = Ptr_RGBX8(b.XMax(),b.YMin()+1);
        int y;
        for( y=b.YMin()+1; y<=b.YMax()-1; ++y )
        {
            *pleft = pen.rgb();
            pleft += W();
            *pright = pen.rgb();
            pright += W();
        }
    } else {
        assert(false);// not implemented yet
    }
}


//-----------------------




// clips a blit against the destination boundary.
// assumes srcbox is already valid.
// modifies srcbox and destbox appropriately.
static void clip_blit(
    Box const& srcbounds, Box& srcbox,
    Box const& destbounds, Box& destbox,
    int zoom=1 )
{
    // srcbox has blit dimensions
    destbox.w = srcbox.w*zoom;
    destbox.h = srcbox.h*zoom;

    // clip destbox, then adjust srcbox to take into account any
    // modifications
    int destx = destbox.x;
    int desty = destbox.y;
    destbox.ClipAgainst( destbounds );

    srcbox.x += (destbox.x - destx)/zoom;
    srcbox.y += (destbox.y - desty)/zoom;
    srcbox.w = destbox.w/zoom;
    srcbox.h = destbox.h/zoom;
}





void BlitTransparent(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    PenColour const& transparentcolour )
{
    assert(srcimg.Fmt()==destimg.Fmt());

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

                int x;
                for( x=0; x<destclipped.w; ++x )
                {
                    I8 c = *src++;
                    if( c != transparentcolour.idx() )
                        *dest = c;
                    ++dest;
                }
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.x+0, srcclipped.y+y );
                RGBX8* dest = destimg.Ptr_RGBX8( destclipped.x+0, destclipped.y+y );

                int x;
                for( x=0; x<destclipped.w; ++x )
                {
                    RGBX8 c = *src++;
                    if( c != transparentcolour.rgb() )
                        *dest = c;
                    ++dest;
                }
            }
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
    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped );

    int y;
    for( y=0; y<destclipped.h; ++y )
    {
        switch(destimg.Fmt())
        {
        case FMT_I8:
            {
                assert(mattecolour.IdxValid()); // can't blit rgb colour onto indexed image!

                I8* dest = destimg.Ptr_I8( destclipped.x+0, destclipped.y+y );
                if(srcimg.Fmt()==FMT_I8)
                {
                    I8 const* src = srcimg.PtrConst_I8( srcclipped.x+0, srcclipped.y+y );
                    int x;
                    for( x=0; x<destclipped.w; ++x )
                    {
                        I8 c = *src++;
                        if( c != transparentcolour.idx() )
                            *dest = mattecolour.idx();
                        ++dest;
                    }
                }
                else if(srcimg.Fmt()==FMT_RGBX8)
                {
                    RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.x+0, srcclipped.y+y );
                    int x;
                    for( x=0; x<destclipped.w; ++x )
                    {
                        RGBX8 c = *src++;
                        if( c != transparentcolour.rgb() )
                            *dest = mattecolour.idx();
                        ++dest;
                    }
                }
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8* dest = destimg.Ptr_RGBX8( destclipped.x+0, destclipped.y+y );
                if(srcimg.Fmt()==FMT_I8)
                {
                    I8 const* src = srcimg.PtrConst_I8( srcclipped.x+0, srcclipped.y+y );
                    int x;
                    for( x=0; x<destclipped.w; ++x )
                    {
                        I8 c = *src++;
                        if( c != transparentcolour.idx() )
                            *dest = mattecolour.rgb();
                        ++dest;
                    }
                }
                else if(srcimg.Fmt()==FMT_RGBX8)
                {
                    RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.x+0, srcclipped.y+y );
                    int x;
                    for( x=0; x<destclipped.w; ++x )
                    {
                        RGBX8 c = *src++;
                        if( c != transparentcolour.rgb() )
                            *dest = mattecolour.rgb();
                        ++dest;
                    }
                }
            }
            break;
        default:
            assert(false);
            break;
        }
    }
    destbox = destclipped;
}



void BlitZoomTransparent(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    Palette const& palette,
    int zoom,
    PenColour const& transparentcolour)
{
    assert( destimg.Fmt()==FMT_RGBX8);
    assert( srcimg.Bounds().Contains( srcbox ) );
    assert( zoom >= 1 );

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped, zoom );

    int y;
    for( y=0; y<destclipped.H(); ++y )
    {
        int x;
        RGBX8* dest = destimg.Ptr_RGBX8( destclipped.XMin() + 0, destclipped.YMin() + y );
        switch(srcimg.Fmt())
        {
        case FMT_I8:
            {
                I8 const* src = srcimg.PtrConst_I8( srcclipped.XMin()+0, srcclipped.YMin()+y/zoom );
                int n=0;
                for( x=0; x<destclipped.W(); ++x )
                {
                    if( *src != transparentcolour.idx())
                    {
                        RGBX8 c = palette.GetColour(*src);
                        *dest = c;
                    }
                    ++dest;
                    if( ++n >= zoom )
                    {
                        ++src;
                        n=0;
                    }
                }
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.XMin()+0, srcclipped.YMin()+y/zoom );
                int n=0;
                for( x=0; x<destclipped.W(); ++x )
                {
                    RGBX8 c=*src;
                    if( c != transparentcolour.rgb())
                        *dest = c;
                    ++dest;
                    if( ++n >= zoom )
                    {
                        ++src;
                        n=0;
                    }
                }
            }
            break;

        }
    }
}


void BlitZoomMatte(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    Palette const& palette,
    int zoom,
    PenColour const& transparentcolour,
    PenColour const& mattecolour )
{
    assert( destimg.Fmt()==FMT_RGBX8);
    assert( srcimg.Bounds().Contains( srcbox ) );
    assert( zoom >= 1 );

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped, zoom );

    int y;
    for( y=0; y<destclipped.H(); ++y )
    {
        int x;
        RGBX8* dest = destimg.Ptr_RGBX8( destclipped.XMin() + 0, destclipped.YMin() + y );
        switch(srcimg.Fmt())
        {
        case FMT_I8:
            {
                I8 const* src = srcimg.PtrConst_I8( srcclipped.XMin()+0, srcclipped.YMin()+y/zoom );
                int n=0;
                for( x=0; x<destclipped.W(); ++x )
                {
                    if( *src != transparentcolour.idx())
                        *dest = mattecolour.rgb();
                    ++dest;
                    if( ++n >= zoom )
                    {
                        ++src;
                        n=0;
                    }
                }
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.XMin()+0, srcclipped.YMin()+y/zoom );
                int n=0;
                for( x=0; x<destclipped.W(); ++x )
                {
                    if( *src != transparentcolour.rgb())
                        *dest = mattecolour.rgb();
                    ++dest;
                    if( ++n >= zoom )
                    {
                        ++src;
                        n=0;
                    }
                }
            }
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
        default:
            assert(false);
            break;
        }
    }

    destbox = destclipped;
}


