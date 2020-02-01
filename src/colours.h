#ifndef COLOURS_H
#define COLOURS_H

#include <stdint.h>
#include <cassert>


// Raw pixel types:
enum PixelFormat {
    FMT_I8=0,
    FMT_RGBX8,  // rgb only, alpha ignored
    FMT_RGBA8,
};

class RGBA8;

// Colour, 8 bits/channel
struct RGBX8
{
    RGBX8() {}
    RGBX8(uint8_t red, uint8_t green, uint8_t blue) : b(blue), g(green), r(red), pad(255) {}
    // KLUDGE: this order for QT version (QImage ARGB fmt assumes native byte order...)
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t pad;    // should always be 255
};

inline bool operator==(const RGBX8& a, const RGBX8& b){ return a.r==b.r && a.g==b.g && a.b==b.b; } 
inline bool operator!=(const RGBX8& a, const RGBX8& b){return !operator==(a,b);}

// RGBA 8 bits/channel
struct RGBA8
{
    RGBA8() {}
    RGBA8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : b(blue),g(green),r(red),a(alpha) {}
    RGBA8(RGBX8 rgb) : b(rgb.b),g(rgb.g),r(rgb.r),a(255) {}
    // KLUDGE: this order for QT version (QImage ARGB fmt assumes native byte order...)
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
};

inline bool operator==(const RGBA8& a, const RGBA8& b){ return a.r==b.r && a.g==b.g && a.b==b.b && a.a==b.a; }
inline bool operator!=(const RGBA8& a, const RGBA8& b){return !operator==(a,b);}


inline RGBX8 Blend(RGBA8 src, RGBX8 dest)
{
    uint8_t t = src.a;
    uint8_t inv = 255-src.a;
    return RGBX8(
        (dest.r*inv + src.r*t)/255,
        (dest.g*inv + src.g*t)/255,
        (dest.b*inv + src.b*t)/255 );
}

inline RGBA8 Blend(RGBA8 src, RGBA8 dest)
{
    uint8_t t = src.a;
    uint8_t inv = 255-src.a;
    return RGBA8(
        (dest.r*inv + src.r*t)/255,
        (dest.g*inv + src.g*t)/255,
        (dest.b*inv + src.b*t)/255,
        (dest.a*inv + src.a*t)/255 );
}
/*
inline RGBX8 Lerp(RGBX8 a, RGBX8 b, uint8_t t) {
    uint8_t inv = 255-t;
    RGBX8 out;
    out.r = (a.r*inv + b.r*t)/255;
    out.g = (a.g*inv + b.g*t)/255;
    out.b = (a.b*inv + b.b*t)/255;
    return out;
}
*/

// indexed, 8-bit
typedef uint8_t I8;



// higher-level colour handling:
struct Colour
{
    Colour(uint8_t red=0, uint8_t green=0, uint8_t blue=0, uint8_t alpha=255 ) :
        b(blue),
        g(green),
        r(red),
        a(alpha)
    {
    }
    Colour(RGBX8 raw) :
        b(raw.b),
        g(raw.g),
        r(raw.r),
        a(255)
    {
    }
    Colour(RGBA8 raw) :
        b(raw.b),
        g(raw.g),
        r(raw.r),
        a(raw.a)
    {
    }

    // this order for QT version (QImage ARGB fmt assumes native byte order...)
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;

    operator RGBX8() const
        { RGBX8 tmp; tmp.r=r; tmp.g=g; tmp.b=b; tmp.pad=255; return tmp; }
    operator RGBA8() const
        { RGBA8 tmp; tmp.r=r; tmp.g=g; tmp.b=b; tmp.a=a; return tmp; }
};

inline bool operator==(const Colour& a, const Colour& b) { return a.r==b.r && a.g==b.g && a.b==b.b &&a.a==b.a; } 
inline bool operator!=(const Colour& a, const Colour& b) { return !(a == b);}
inline bool operator<(Colour const& lhs, Colour const& rhs) {
    if (lhs.r != rhs.r) {
        return lhs.r < rhs.r;
    }
    if (lhs.g != rhs.g) {
        return lhs.g < rhs.g;
    }
    if (lhs.b != rhs.b) {
        return lhs.b < rhs.b;
    }
    return lhs.a < rhs.a;
}

inline int DistSq(Colour const& a, Colour const& b) {
    return (b.r - a.r) * (b.r - a.r) +
        (b.g - a.g) * (b.g - a.g) +
        (b.b - a.b) * (b.b - a.b) +
        (b.a - a.a) * (b.a - a.a);
}


inline Colour Lerp(Colour const& a, Colour const& b, float t)
{
    assert(t>=0.0f && t <= 1.0f);
    float inv = 1-t;
    return Colour(
        (int)(a.r*inv + b.r*t),
        (int)(a.g*inv + b.g*t),
        (int)(a.b*inv + b.b*t),
        (int)(a.a*inv + b.a*t));
}

bool ParseHexColour(const char* in, Colour& out);


// a colour value to pass into drawing functions, which can cope with
// both indexed and non-indexed drawing.
// rgb is _always_ set, but index may not be.
// So any pen can draw to an rgb image, but only a pen with a valid
// index can draw to a paletted image. (drawing functions should check
// this with asserts)
class PenColour
{
public:
    PenColour() :
        m_rgb(255,0,255),
        m_idx(-1)
    {
    }

    PenColour( Colour c, int i=-1) :
        m_rgb(c),
        m_idx(i)
    {
    }

    Colour rgb() const {return m_rgb; }
    int idx() const { assert(IdxValid());return m_idx; }

    bool IdxValid() const { return m_idx>=0; }
private:
    Colour m_rgb;
    int m_idx; // -1 = invalid
};


#endif // COLOURS_H

