#include "ranges.h"


int RangeGrid::UpdatePen(int idx, Colour const& c)
{
    int cnt = 0;
    for (size_t i=0; i<m_Valid.size(); ++i) {
        if (!m_Valid[i]) {
            continue;
        }
        PenColour& pen = m_Pens[i];
        if (pen.IdxValid() && pen.idx() == idx) {
            pen = PenColour(c, idx);
            ++cnt;
        }
    }
    return cnt;
}



Box RangeGrid::PickRange(Point const& pos) const
{
    if (!IsSet(pos)) {
        return Box(0, 0, 0, 0);
    }

    Box vert(pos.x, pos.y, 1, 1);
    // scan upward to find start
    while (IsSet(Point(vert.x, vert.YMin() - 1))) {
        --vert.y;
        ++vert.h;
    }
    // scan for bottom
    while (IsSet(Point(vert.x, vert.YMax() + 1))) {
        ++vert.h;
    }
    if (vert.h > 1 ) {
        return vert;
    }

    // no vertical range found. try horizontal.
    Box horiz(pos.x, pos.y, 1, 1);
    // scan left to find start
    while (IsSet(Point(horiz.XMin()-1, horiz.y))) {
        --horiz.x;
        ++horiz.w;
    }
    // scan for right
    while (IsSet(Point(horiz.XMax()+1, horiz.y))) {
        ++horiz.w;
    }

    if (horiz.w > 1 ) {
        return horiz;
    }

    return Box(0, 0, 0, 0);
}

