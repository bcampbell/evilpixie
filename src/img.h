#ifndef IMG_H
#define IMG_H

#include "box.h"
#include "colours.h"
#include "point.h"

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
class IndexedImg
{
public:
    IndexedImg();   // disallowed
    IndexedImg( IndexedImg const& other );
    IndexedImg( IndexedImg const& other, Box const& otherarea );
	IndexedImg( int w, int h, uint8_t const* initial=0 );

    // disallowed (use Copy() instead!)
    IndexedImg& operator=( IndexedImg const& other );

	virtual ~IndexedImg();

	int W() const
		{ return m_Bounds.w; }
	int H() const
		{ return m_Bounds.h; }
	uint8_t* Ptr( int x=0, int y=0 )
		{ return m_Pixels + y*W() + x; }
	uint8_t const* PtrConst( int x=0, int y=0 ) const
		{ return m_Pixels + y*W() + x; }
    Box const& Bounds() const
        { return m_Bounds; }

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

    void Copy( IndexedImg const& other );


private:
    Box m_Bounds;   // only w,h used. x,y always 0
	uint8_t* m_Pixels;

};




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
    RGBx const* palette,
    int zoom,
    int transparentcolour=-1,
    int maskcolour=-1 );

#endif // IMG_H

