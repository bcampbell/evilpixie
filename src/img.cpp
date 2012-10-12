#include "img.h"
#include "palette.h"
#include "draw.h"

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


