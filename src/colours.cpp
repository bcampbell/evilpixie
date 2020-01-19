#include "colours.h"

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

