#include "brush.h"



Brush::Brush( BrushStyle style, int w, int h, uint8_t const* initial, int transparent ) :
    IndexedImg( w,h,initial ),
    m_Style(style),
    m_Handle( w/2, h/2 ),
    m_Transparent(transparent)
{
}

Brush::~Brush()
{
}

