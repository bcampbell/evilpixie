#ifndef BRUSH_H
#define BRUSH_H

#include "img.h"
#include "palette.h"


enum BrushStyle { MASK, FULLCOLOUR };

class Brush : public Img        // TODO: UGH. no no no.
{
public:
    Brush( BrushStyle style, int w, int h, uint8_t const* initial, PenColour transparent );

    // copy from an image
    Brush( BrushStyle style, Img const& src, Box const& area, PenColour transparent );
    virtual ~Brush();

    Point const& Handle() const     { return m_Handle; }
    void SetHandle( Point const& p ) { m_Handle=p; }
    PenColour TransparentColour() const   { return m_Transparent; }
    BrushStyle Style() const          { return m_Style; }

    // TODO: image could carry it's own palette
    Palette const& GetPalette() const
        { return m_Palette; }

    void SetPalette( Palette const& pal )
        { m_Palette = pal; }

private:
    BrushStyle m_Style;
    Point m_Handle;
    PenColour m_Transparent;

    Palette m_Palette;
};


#endif //BRUSH_H

