#ifndef COLOURS_H
#define COLOURS_H

#include <stdint.h>
#include <cassert>


// Raw pixel types:
enum PixelFormat {
    FMT_I8=0,
    FMT_RGBX8,
};


// RGBx
struct RGBX8
{
    // KLUDGE: this order for QT version (QImage ARGB fmt assumes native byte order...)
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t pad;
};
inline bool operator==(const RGBX8& a, const RGBX8& b){ return a.r==b.r && a.g==b.g && a.b==b.b; } 
inline bool operator!=(const RGBX8& a, const RGBX8& b){return !operator==(a,b);}

// indexed, 8-bit
typedef uint8_t I8;


union VColour
{
    RGBX8 rgbx;
    I8 i;
	VColour( uint8_t red=0, uint8_t green=0, uint8_t blue=0 ) {
        rgbx.r=red;
        rgbx.g=green;
        rgbx.b=blue;
        rgbx.pad=255;
    }
};


// higher-level colour handling:
// TODO: rename to Colour or something
struct RGBx
{
    RGBx(uint8_t red=0, uint8_t green=0, uint8_t blue=8 ) :
        b(blue),
        g(green),
        r(red),
        pad(255)
    {
    }
    RGBx(RGBX8 raw) :
        b(raw.b),
        g(raw.g),
        r(raw.r),
        pad(255)
    {
    }

    // this order for QT version (QImage ARGB fmt assumes native byte order...)
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t pad;

    operator RGBX8() const
        { RGBX8 tmp; tmp.r=r; tmp.g=g; tmp.b=b; tmp.pad=255; return tmp; }
    operator VColour() const
        { VColour tmp; tmp.rgbx = (RGBX8)*this; return tmp; }
};

inline RGBx Lerp(RGBx const& a, RGBx const& b, float t)
{
    assert(t>=0.0f && t <= 1.0f);
    float inv = 1-t;
    return RGBx(
        (int)(a.r*inv + b.r*t),
        (int)(a.g*inv + b.g*t),
        (int)(a.b*inv + b.b*t));
}


#endif // COLOURS_H

