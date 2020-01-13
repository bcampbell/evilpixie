
#include <algorithm>
#include <cmath>
#include <cassert>

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
    float c = v*s;  // chroma
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
    float m = v-c;
    r+=m;
    g+=m;
    b+=m;
}



