#pragma once

#include "box.h"
#include "colours.h"
#include "point.h"

#include <vector>

struct Range {
    std::vector<PenColour> pens;
    // Cosmetic stuff. For editing ranges with pretty layout.
    Point pos;
    bool horizontal;

};



class RangeGrid {
public:
    RangeGrid(int w, int h);

    // Points outside grid are considered unset.
    bool Get(Point const& pos, PenColour& out) const;
    bool IsSet(Point const& pos) const;

    // Set/Clear will assert if pos is outside grid.
    void Set(Point const& pos, PenColour const& pen);
    void Clear(Point const& pos);
    Box const& Bound() const { return m_Bound; }

    // Update the rgb of any pens that might be using idx.
    // Returns number of pens updated.
    int UpdatePen(int idx, Colour const& c);

    Box PickRange(Point const& pos) const;

private:
    Box m_Bound;
    std::vector<bool> m_Valid;
    std::vector<PenColour> m_Pens;
};

inline RangeGrid::RangeGrid(int w, int h) :
    m_Bound(0, 0, w, h),
    m_Valid(w * h, false),
    m_Pens(w * h)
{
}

inline bool RangeGrid::Get(Point const& pos, PenColour& out) const
{
    if (!m_Bound.Contains(pos)) {
        return false;
    }
    size_t idx = ((pos.y-m_Bound.y) * m_Bound.w) + (pos.x - m_Bound.x);
    if (!m_Valid[idx]) {
        return false;
    }
    out = m_Pens[idx];
    return true;
}

inline bool RangeGrid::IsSet(Point const& pos) const
{
    if (!m_Bound.Contains(pos)) {
        return false;
    }
    size_t idx = ((pos.y-m_Bound.y) * m_Bound.w) + (pos.x - m_Bound.x);
    return m_Valid[idx];
}


inline void RangeGrid::Set(Point const& pos, PenColour const& pen)
{
    assert(m_Bound.Contains(pos));
    size_t idx = ((pos.y-m_Bound.y) * m_Bound.w) + (pos.x - m_Bound.x);
    m_Valid[idx] = true;
    m_Pens[idx] = pen;
}

inline void RangeGrid::Clear(Point const& pos)
{
    assert(m_Bound.Contains(pos));
    size_t idx = ((pos.y-m_Bound.y) * m_Bound.w) + (pos.x - m_Bound.x);
    m_Valid[idx] = false;
}

