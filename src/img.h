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
    enum Fmt {
        INDEXED8BIT=0,
        RGBx
    };
    Img();   // disallowed
    Img( Img const& other );
    Img( Img const& other, Box const& otherarea );

	Img( Fmt pixel_format, int w, int h, uint8_t const* initial=0 );
    // disallowed (use Copy() instead!)
    Img& operator=( Img const& other );

	~Img()
		{ delete [] m_Pixels; }
    Fmt Format() const { return m_Format; }
	int W() const
		{ return m_Bounds.w; }
	int H() const
		{ return m_Bounds.h; }
	uint8_t* Ptr( int x, int y )
		{ return m_Pixels + (y*m_BytesPerRow) + (x*m_BytesPerPixel); }
	uint8_t const* PtrConst( int x, int y ) const
		{ return m_Pixels + (y*m_BytesPerRow) + (x*m_BytesPerPixel); }
    Box const& Bounds() const
        { return m_Bounds; }
#if 0
	void SetPixel( int x, int y, RGBx c )
		{ RGBx* p=Ptr(x,y); *p=c; }

    // FillBox & Outlinebox handle clipping.
    // b will return area affected after clipping.
	void FillBox( RGBx c, Box& b );
    void OutlineBox( RGBx c, Box& b );
#endif
    void Copy( Img const& other );

    friend void ::Blit(
        Img const& srcimg,
        Box const& srcbox,
        Img& destimg,
        Box& destbox);


protected:
    void init();

    Fmt m_Format;
    int m_BytesPerPixel;
    int m_BytesPerRow;
    Box m_Bounds;
	uint8_t* m_Pixels;
};


// An RGB bitmap
class RGBImg
{
public:
	RGBImg( int w, int h ) :
		m_Bounds(0,0,w,h),
		m_Pixels( new RGBx[w*h] )
		{}

	~RGBImg()
		{ delete [] m_Pixels; }

	int W() const
		{ return m_Bounds.w; }
	int H() const
		{ return m_Bounds.h; }
	RGBx* Ptr( int x, int y )
		{ return m_Pixels + y*W() + x; }
	RGBx const* PtrConst( int x, int y ) const
		{ return m_Pixels + y*W() + x; }
    Box const& Bounds() const
        { return m_Bounds; }

	void SetPixel( int x, int y, RGBx c )
		{ RGBx* p=Ptr(x,y); *p=c; }

    // FillBox & Outlinebox handle clipping.
    // b will return area affected after clipping.
	void FillBox( RGBx c, Box& b );
    void OutlineBox( RGBx c, Box& b );

private:
    Box m_Bounds;
	RGBx* m_Pixels;
};


// an indexed image
class IndexedImg : public Img
{
public:
    IndexedImg();   // disallowed
	IndexedImg( int w, int h, uint8_t const* initial=0 );
    IndexedImg( IndexedImg const& other, Box const& otherarea ) : Img(other,otherarea) {}

    // disallowed (use Copy() instead!)
    IndexedImg& operator=( IndexedImg const& other );

	void SetPixel( int x, int y, uint8_t c )
		{ uint8_t* p=Ptr(x,y); *p=c; }
	uint8_t GetPixel( int x, int y ) const
		{ return *PtrConst(x,y); }
	void SetPixel( const Point& p, uint8_t c )
		{ *Ptr(p.x,p.y) = c; }
	uint8_t GetPixel( const Point& p ) const
		{ return *PtrConst(p.x,p.y); }

    // b will return area affected after clipping.
	void FillBox( uint8_t c, Box& b );


    void XFlip();
    void YFlip();
private:
};



void Blit(
    Img const& srcimg,
    Box const& srcbox,
    Img& destimg,
    Box& destbox);


// destbox is changed to reflect the final clipped area on the dest Img
void BlitIndexed(
    IndexedImg const& srcimg, Box const& srcbox,
    IndexedImg& destimg, Box& destbox,
    int transparentcolour = -1,
    int maskcolour = -1 );


// Same as BlitIndexed, except that srcimg is replaced by destimg
// (all pixels, not just the non-transparent ones)
void BlitSwapIndexed(
    IndexedImg& srcimg, Box const& srcbox,
    IndexedImg& destimg, Box& destbox,
    int transparentcolour = -1,
    int maskcolour = -1 );

void BlitZoomIndexedToRGBx(
    IndexedImg const& srcimg, Box const& srcbox,
    RGBImg& destimg, Box& destbox,
    Palette const& palette,
    int zoom,
    int transparentcolour=-1,
    int maskcolour=-1 );

#endif // IMG_H

