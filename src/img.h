#ifndef IMG_H
#define IMG_H

#include "box.h"
#include "colours.h"
#include "point.h"

#include <cassert>

class Palette;



class Img
{
public:
    Img();   // disallowed
    Img( Img const& other );
    Img( Img const& other, Box const& otherarea );

	Img( PixelFormat pixel_format, int w, int h, uint8_t const* initial=0 );
    // disallowed (use Copy() instead!)
    Img& operator=( Img const& other );

	~Img()
		{ delete [] m_Pixels; }
    PixelFormat Fmt() const { return m_Format; }
	int W() const
		{ return m_Bounds.w; }
	int H() const
		{ return m_Bounds.h; }
    // TODO: Kill or hide
	uint8_t* Ptr( int x, int y )
		{ return m_Pixels + (y*m_BytesPerRow) + (x*m_BytesPerPixel); }
    // TODO: Kill or hide
	uint8_t const* PtrConst( int x, int y ) const
		{ return m_Pixels + (y*m_BytesPerRow) + (x*m_BytesPerPixel); }

	I8* Ptr_I8( int x, int y )
		{ assert(Fmt()==FMT_I8); return (I8*)Ptr(x,y); }
	I8 const* PtrConst_I8( int x, int y ) const
		{ assert(Fmt()==FMT_I8); return (I8*)PtrConst(x,y); }

	RGBX8* Ptr_RGBX8( int x, int y )
		{ assert(Fmt()==FMT_RGBX8); return (RGBX8*)Ptr(x,y); }
	RGBX8 const* PtrConst_RGBX8( int x, int y ) const
		{ assert(Fmt()==FMT_RGBX8); return (RGBX8*)PtrConst(x,y); }

    Box const& Bounds() const
        { return m_Bounds; }

    // TODO: move all drawing ops out to somewhere else...
    void HLine( VColour pen, int xbegin, int xend, int y);
    void Copy( Img const& other );
    // b will return area affected after clipping.
	void FillBox( VColour pen, Box& b );
    void OutlineBox( VColour pen, Box& b );

    void XFlip();
    void YFlip();
	void SetPixel( int x, int y, uint8_t c )
		{ assert(Fmt()==FMT_I8); uint8_t* p=Ptr(x,y); *p=c; }
	uint8_t GetPixel( int x, int y ) const
		{ assert(Fmt()==FMT_I8); return *PtrConst(x,y); }
	void SetPixel( const Point& p, uint8_t c )
		{ assert(Fmt()==FMT_I8); *Ptr(p.x,p.y) = c; }
	uint8_t GetPixel( const Point& p ) const
		{ assert(Fmt()==FMT_I8); return *PtrConst(p.x,p.y); }

    friend void ::Blit(
        Img const& srcimg,
        Box const& srcbox,
        Img& destimg,
        Box& destbox);


protected:
    void init();

    PixelFormat m_Format;
    int m_BytesPerPixel;
    int m_BytesPerRow;
    Box m_Bounds;
	uint8_t* m_Pixels;
};



void Blit(
    Img const& srcimg,
    Box const& srcbox,
    Img& destimg,
    Box& destbox);


// destbox is changed to reflect the final clipped area on the dest Img
void BlitFancy(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    int transparentcolour = -1,
    int maskcolour = -1 );


// Same as Blit, except that srcimg is replaced by destimg
void BlitSwap(
    Img& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox);

void BlitZoomIndexedToRGBx(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    Palette const& palette,
    int zoom,
    int transparentcolour=-1,
    int maskcolour=-1 );

#endif // IMG_H

