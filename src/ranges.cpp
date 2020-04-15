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
