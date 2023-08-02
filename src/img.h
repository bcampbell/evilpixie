#ifndef IMG_H
#define IMG_H

#include "box.h"
#include "colours.h"
#include "point.h"

#include <cassert>

struct Palette;



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

	I8* Ptr_I8( int x, int y )
		{ assert(Fmt()==FMT_I8); return (I8*)Ptr(x,y); }
	I8 const* PtrConst_I8( int x, int y ) const
		{ assert(Fmt()==FMT_I8); return (I8*)PtrConst(x,y); }

	RGBX8* Ptr_RGBX8( int x, int y )
		{ assert(Fmt()==FMT_RGBX8); return (RGBX8*)Ptr(x,y); }
	RGBX8 const* PtrConst_RGBX8( int x, int y ) const
		{ assert(Fmt()==FMT_RGBX8); return (RGBX8*)PtrConst(x,y); }

	RGBA8* Ptr_RGBA8( int x, int y )
		{ assert(Fmt()==FMT_RGBA8); return (RGBA8*)Ptr(x,y); }
	RGBA8 const* PtrConst_RGBA8( int x, int y ) const
		{ assert(Fmt()==FMT_RGBA8); return (RGBA8*)PtrConst(x,y); }

    // Raw access.
	uint8_t* Ptr( int x, int y )
		{ return m_Pixels + (y*m_BytesPerRow) + (x*m_BytesPerPixel); }
	uint8_t const* PtrConst( int x, int y ) const
		{ return m_Pixels + (y*m_BytesPerRow) + (x*m_BytesPerPixel); }
    int Pitch() const
        { return m_BytesPerRow; }

    Box const& Bounds() const
        { return m_Bounds; }

    // TODO: move all drawing ops out to somewhere else...
    void HLine( PenColour const& pen, int xbegin, int xend, int y);
    // rename to Clone
    void Copy( Img const& other );
    // b will return area affected after clipping.
	void FillBox( PenColour const& pen, Box& b );
    void OutlineBox( PenColour const& pen, Box& b );

    void XFlip();
    void YFlip();

    // helpers to get single pixel
	RGBX8 Get_RGBX8( const Point& p ) const
		{ return *PtrConst_RGBX8(p.x,p.y); }
	RGBA8 Get_RGBA8( const Point& p ) const
		{ return *PtrConst_RGBA8(p.x,p.y); }
	I8 Get_I8( const Point& p ) const
		{ return *PtrConst_I8(p.x,p.y); }


protected:
    void init();

    PixelFormat m_Format;
    int m_BytesPerPixel;
    int m_BytesPerRow;
    Box m_Bounds;   // TODO: should just be w & h.
	uint8_t* m_Pixels;
private:
};

// Return a copy of the image, rotated 90 degrees clockwise.
Img* Rotate90Clockwise(Img const& srcImg);

#endif // IMG_H

