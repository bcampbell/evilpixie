#pragma once

#include "box.h"
#include "colours.h"
#include "point.h"

#include <cstddef>  // for size_t
#include <vector>


class Palette;

// Free-form colour ranges. A grid, in which pens can be placed.
// Ranges are any continuous run of two or more entries on the grid,
// either horizontal or vertical.
// So two runs can share a position, crossword-style.
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
    // (so we can update the rgb values for pens when the palette is modified)
    // Returns number of pens updated.
    int UpdatePen(int idx, Colour const& c);

    // Update all ranges to cope with a new palette.
    // Just updates the rgb values of in-range pens, and invalidate any
    // which are now off the end of the new palette.
    // Returns the number of entries affected.
    int UpdateAll(Palette const& newPalette);

    // Remap ranges to new palette.
    // Returns the number of entries affected.
    int Remap(Palette const& newPalette);

    // Return the first range found which passes through pos.
    // (there might be both vertical and horizontal. Undefined which one will
    // be returned). Returned box will be either w=1 (vertical range),
    // h=1 (horizontal range) or empty (no range).
    Box PickRange(Point const& pos) const;

    // Return true if the point is shared by more than one range.
    // (eg intersection of horizontal and vertical ranges)
    bool IsShared(Point const& pos) const;

    // Read out the pens in the given range.
    // (ok if range is empty - out will be empty).
    void FetchPens(Box const& range, std::vector<PenColour>& out) const;

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

