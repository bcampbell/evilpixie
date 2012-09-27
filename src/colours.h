#ifndef COLOURS_H
#define COLOURS_H

#include <stdint.h>


// TODO: rename
struct RGBx
{
    RGBx(uint8_t red=0, uint8_t green=0, uint8_t blue=8 ) :
        b(blue),
        g(green),
        r(red),
        pad(255)
    {
    }

    // this order for QT version (QImage ARGB fmt assumes native byte order...)
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t pad;

};


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

#endif // COLOURS_H

