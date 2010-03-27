#ifndef COLOURS_H
#define COLOURS_H

#include <stdint.h>

class RGBx
{
public:
	RGBx( uint8_t red=0, uint8_t green=0, uint8_t blue=0 ) :
        b(blue),
        g(green),
		r(red),
        pad(255)
        {}
    // this order for QT version (QImage ARGB fmt assumes native byte order...)
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t pad;
};

#endif // COLOURS_H

