#include "img.h"
#include "palette.h"

#include <cstring>
#include <cstdio>
#include <cassert>
#include <algorithm>    // for reverse()

Img::Img( Fmt pixel_format, int w, int h, uint8_t const* initial ) :
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
        case INDEXED8BIT: m_BytesPerPixel=1; break;
        case RGBx: m_BytesPerPixel=4; break;
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





void RGBImg::FillBox( RGBx c, Box& b )
{
    b.ClipAgainst( Bounds() );

    int y;
    for( y=b.YMin(); y<=b.YMax(); ++y )
    {
        int x;
        RGBx* dest = Ptr(b.XMin(),y);
        for( x=b.XMin(); x<=b.XMax(); ++x )
        {
            *dest++ = c;
        }
    }
}

void RGBImg::OutlineBox( RGBx c, Box& b )
{
    b.ClipAgainst( Bounds() );
    // draw top & bottom
    int x;
    RGBx* ptop = Ptr(b.XMin(),b.YMin());
    RGBx* pbot = Ptr(b.XMin(),b.YMax());
    for( x=b.XMin(); x<=b.XMax(); ++x )
    {
        *ptop++ = c;
        *pbot++ = c;
    }

    // draw sides (note: already draw top & bottom pixels)
    RGBx* pleft = Ptr(b.XMin(),b.YMin()+1);
    RGBx* pright = Ptr(b.XMax(),b.YMin()+1);
    int y;
    for( y=b.YMin()+1; y<=b.YMax()-1; ++y )
    {
        *pleft = c;
        pleft += W();
        *pright = c;
        pright += W();
    }
}


//-----------------------

IndexedImg::IndexedImg(int w, int h, uint8_t const* initial) :
    Img(INDEXED8BIT, w, h, initial)
{
}




void IndexedImg::FillBox( uint8_t c, Box& b )
{
    b.ClipAgainst( Bounds() );

    int y;
    for( y=b.YMin(); y<=b.YMax(); ++y )
    {
        int x;
        uint8_t* dest = Ptr(b.XMin(),y);
        for( x=b.XMin(); x<=b.XMax(); ++x )
        {
            *dest++ = c;
        }
    }
}


void IndexedImg::XFlip()
{
    int y;
    for(y=0; y<H(); ++y)
    {
        uint8_t* begin = Ptr(0,y);
        uint8_t* end = begin + W();
        std::reverse(begin,end);
    }
}

void IndexedImg::YFlip()
{
    int y;
    for(y=0; y<H()/2; ++y)
    {
        uint8_t* a = Ptr(0,y);
        uint8_t* b = Ptr(0,(H()-1)-y);
        std::swap_ranges(a,a+W(),b);
    }
}


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


void BlitIndexed(
    IndexedImg const& srcimg, Box const& srcbox,
    IndexedImg& destimg, Box& destbox,
    int transparentcolour,
    int maskcolour )
{
    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped );

    int y;
    for( y=0; y<destclipped.h; ++y )
    {
        uint8_t const* src = srcimg.PtrConst( srcclipped.x+0, srcclipped.y+y );
        uint8_t* dest = destimg.Ptr( destclipped.x+0, destclipped.y+y );

        // different innerloops depending on transparency/mask
        if( transparentcolour == -1 )
        {
            // straight blit (maskcolour not supported)
            int x;
            for( x=0; x<destclipped.w; ++x )
                *dest++ = *src++;
        }
        else
        {
            int x;
            for( x=0; x<destclipped.w; ++x )
            {
                uint8_t c = *src++;
                if( c != (uint8_t)transparentcolour )
                {
                    if( maskcolour != -1 )
                        c= (uint8_t)maskcolour;
                    *dest = c;
                }
                ++dest;
            }
        }
    }

    destbox = destclipped;
}



void BlitSwapIndexed(
    IndexedImg& srcimg, Box const& srcbox,
    IndexedImg& destimg, Box& destbox,
    int transparentcolour,
    int maskcolour )
{
    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped );

    int y;
    for( y=0; y<destclipped.h; ++y )
    {
        uint8_t* src = srcimg.Ptr( srcclipped.x+0, srcclipped.y+y );
        uint8_t* dest = destimg.Ptr( destclipped.x+0, destclipped.y+y );

        // different innerloops depending on transparency/mask
        if( transparentcolour == -1 )
        {
            // straight blit (maskcolour not supported)
            int x;
            for( x=0; x<destclipped.w; ++x )
            {
                uint8_t tmp = *dest;
                *dest++ = *src;
                *src++ = tmp;
            }
        }
        else
        {
            int x;
            for( x=0; x<destclipped.w; ++x )
            {
                uint8_t tmp = *dest;
                uint8_t c = *src;
                if( c != (uint8_t)transparentcolour )
                {
                    if( maskcolour != -1 )
                        c= (uint8_t)maskcolour;
                    *dest = c;
                }
                *src++ = tmp;
                ++dest;
            }
        }
    }

    destbox = destclipped;
}


void BlitZoomIndexedToRGBx(
    IndexedImg const& srcimg, Box const& srcbox,
    RGBImg& destimg, Box& destbox,
    Palette const& palette,
    int zoom,
    int transparentcolour,
    int maskcolour )
{
    assert( srcimg.Bounds().Contains( srcbox ) );
    assert( zoom >= 1 );

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped, zoom );

    int y;
    for( y=0; y<destclipped.H(); ++y )
    {
        int x;
        RGBx* dest = destimg.Ptr( destclipped.XMin() + 0, destclipped.YMin() + y );
        uint8_t const* src = srcimg.PtrConst( srcclipped.XMin()+0, srcclipped.YMin()+y/zoom );
        int n=0;
        for( x=0; x<destclipped.W(); ++x )
        {

            RGBx c;// = *dest;
            if( maskcolour != -1 )
                c = palette.GetColour(maskcolour);
            else
                c = palette.GetColour(*src);
            if( *src != transparentcolour )
                *dest = c;
            ++dest;
            if( ++n >= zoom )
            {
                ++src;
                n=0;
            }
        }
    }
}


void Blit(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox)
{
    assert( srcimg.Format() == destimg.Format());

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped );


    int y;
    for( y=0; y<destclipped.h; ++y )
    {
        uint8_t const* src = srcimg.PtrConst( srcclipped.x+0, srcclipped.y+y );
        uint8_t* dest = destimg.Ptr( destclipped.x+0, destclipped.y+y );
//            int x;
//            for( x=0; x<destclipped.w; ++x )
//                *dest++ = *src++;

        memcpy( dest, src, destclipped.w * srcimg.m_BytesPerPixel);
    }

    destbox = destclipped;
}


