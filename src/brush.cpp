#include "brush.h"



Brush::Brush( BrushStyle style, int w, int h, uint8_t const* initial, int transparent ) :
    Img( Img::INDEXED8BIT, w,h,initial ),
    m_Style(style),
    m_Handle( w/2, h/2 ),
    m_Transparent(transparent)
{
}

Brush::Brush( BrushStyle style, Img const& src, Box const& area, int transparent ) :
    Img( src,area ),
    m_Style(style),
    m_Handle( area.w/2, area.h/2 ),
    m_Transparent(transparent)
{
}

Brush::~Brush()
{
}

