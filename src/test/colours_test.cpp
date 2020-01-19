// $ g++ -I .. colours_test.cpp ../colours.cpp
// $ ./a.out || echo "FAILED"

#include "colours.h"

#include <cstdio>

static int fails = 0;

static void check(const char* hex, Colour const& expect) {
    Colour c;
    if (!ParseHexColour(hex,c)) {
        ++fails;
        fprintf(stderr, "Failed to parse '%s'\n", hex);
        return;
    }
    if (c!= expect) {
        ++fails;
        fprintf(stderr, "got %d,%d,%d,%d expected %d,%d,%d,%d",
            c.r, c.b, c.g, c.a,
            expect.r, expect.b, expect.g, expect.a);
        return;
    }
}

static void checkBad(const char* hex) {
    Colour c;
    if (ParseHexColour(hex,c)) {
        ++fails;
        fprintf(stderr, "Didn't reject '%s'\n", hex);
    }
}

int main(int argc, char* argv[]) {

    check("#777", Colour(0x77, 0x77, 0x77, 0xff));
    check("#777777", Colour(0x77, 0x77, 0x77, 0xff));
    check("#aBCd", Colour(0xaa, 0xbb, 0xcc, 0xdd));
    check("#01234567", Colour(0x01, 0x23, 0x45, 0x67));
    checkBad("#12xy45");
    checkBad("#ab");
    checkBad("#ab123");
    checkBad("random words");

    checkBad("6789abcd");   // no leading '#'
    return (fails > 0) ? 1 : 0;
}

