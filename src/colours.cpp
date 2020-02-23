#include "colours.h"

#include <algorithm>
#include <cmath>

// returns false on syntax error
bool ParseHexColour(const char* in, Colour& out) {
    if (*in++ != '#') {
        return false;
    }
    // collect up to 8 hex nibbles
    uint8_t nb[8];
    int i;
    for (i=0; *in && i<8; ++i) {
        char c = *in++;
        if (c>='0' && c<='9') {
            nb[i] = (uint8_t)(c - '0');
        } else if (c>='a' && c<='f') {
            nb[i] = 0xa + (uint8_t)(c - 'a');
        } else if (c>='A' && c<='F') {
            nb[i] = 0xa + (uint8_t)(c - 'A');
        } else {
            return false;
        }
    }

    switch(i) {
        case 3: // #rgb
            out.r = nb[0]<<4 | nb[0];
            out.g = nb[1]<<4 | nb[1];
            out.b = nb[2]<<4 | nb[2];
            out.a = 0xff;
            break;
        case 4: // #rgba
            out.r = nb[0]<<4 | nb[0];
            out.g = nb[1]<<4 | nb[1];
            out.b = nb[2]<<4 | nb[2];
            out.a = nb[3]<<4 | nb[3];
            break;
        case 6: // #rrggbb
            out.r = nb[0]<<4 | nb[1];
            out.g = nb[2]<<4 | nb[3];
            out.b = nb[4]<<4 | nb[5];
            out.a = 0xff;
            break;
        case 8: // #rrggbbaa
            out.r = nb[0]<<4 | nb[1];
            out.g = nb[2]<<4 | nb[3];
            out.b = nb[4]<<4 | nb[5];
            out.a = nb[6]<<4 | nb[7];
            break;
        default:
            return false;
    }
    return true;
}


void RGBToHSV(float r, float g, float b, float& h, float& s, float& v) {
    assert(r >= 0.0f && r <= 1.0f);
    assert(g >= 0.0f && g <= 1.0f);
    assert(b >= 0.0f && b <= 1.0f);
    float fmax = std::max({r,b,g});
    float fmin = std::min({r,b,g});
    float delta = fmax-fmin;
    if (delta>0.0f) {
        if (fmax == r) {
            h = 60.0f * ((g - b) / delta);
        } else if (fmax == g) {
            h = 60.0f * (2.0f + ((b - r) / delta));
        } else if (fmax ==b) {
            h = 60.0f * (4.0f + ((r - g) / delta));
        }
        if (h<0.0f) {
            h += 360.0f;
        }
        //
        s = delta/fmax;
    } else {
        h = 0.0f;
        s = 0.0f;
    }
    v = fmax;
}


void HSVToRGB(float h, float s, float v, float& r, float& g, float& b) {
    assert(s >= 0.0f && s <= 1.0f);
    assert(v >= 0.0f && v <= 1.0f);
    float c = v * s;  // chroma
    if (std::isnan(h)) {
        r = g = b = 0.0f;
    } else {
        assert(h >= 0.0f && h <= 360.0f);
        float hh = h/60.0f;
        float x = c * (1.0f - std::abs(fmod(hh, 2.0f) - 1.0f));
        if (hh<=1.0f) {
            r = c; g = x; b = 0.0f;
        } else if (hh<=2.0f) {
            r = x; g = c; b = 0.0f;
        } else if (hh<=3.0f) {
            r = 0.0f; g = c; b = x;
        } else if (hh<=4.0f) {
            r = 0.0f; g = x; b = c;
        } else if (hh<=5.0f) {
            r = x; g = 0.0f; b = c;
        } else if (hh<=6.0f) {
            r = c; g = 0.0f; b = x;
        } else {
            r = g = b = 0.0f;
            assert(false);  // shouldn't get here!
        }
    }
    float m = v - c;
    r += m;
    g += m;
    b += m;
}


