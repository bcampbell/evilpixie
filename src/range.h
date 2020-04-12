#pragma once

#include "colours.h"
#include <vector>

struct Range {
    std::vector<PenColour> pens;
    // Cosmetic stuff. For editing ranges with pretty layout.
    int x;
    int y;
    bool horizontal;
};


