#include "cmd.h"
#include "project.h"
#include <assert.h>

Cmd_Draw::Cmd_Draw( Project& proj, Box const& affected, IndexedImg const& undoimg ) :
    Cmd( proj, DONE ),
    m_Img( undoimg, affected ),
    m_Affected( affected )
{

}

void Cmd_Draw::Do()
{
    assert( State() == NOT_DONE );
    Box dirty( m_Affected );
    BlitSwapIndexed( m_Img, m_Img.Bounds(), Proj().Img(), dirty );
    Proj().Damage( dirty );
    SetState( DONE );
}

void Cmd_Draw::Undo()
{
    assert( State() == DONE );
    Box dirty( m_Affected );
    BlitSwapIndexed( m_Img, m_Img.Bounds(), Proj().Img(), dirty );
    Proj().Damage( dirty );
    SetState( NOT_DONE );
}

#if 0

Cmd_PlonkBrush::Cmd_PlonkBrush( Project& proj, Point const& pos, Brush const& brush, BrushStyle style, uint8_t pen ) :
    Cmd( proj ),
    m_Pos(pos - brush.Handle() ),
    m_Img(brush),
    m_Transparent( brush.TransparentColour() ),
    m_Style(FULLCOLOUR),
    m_Pen(pen)
{
    // If brush is a mask brush, then we force style to MASK
    if( style==MASK || brush.Style()==MASK )
        m_Style=MASK;
}


void Cmd_PlonkBrush::Do()
{
    Box r( m_Pos.x, m_Pos.y, m_Img.W(), m_Img.H() );

    if( m_Style == MASK )
    {
        // draw it using supplied pen colour
        BlitSwapIndexed( m_Img, m_Img.Bounds(), Proj().Img(), r, m_Transparent, m_Pen );
    }
    else    // FULLCOLOUR
    {
        // use colours from brush image
        BlitSwapIndexed( m_Img, m_Img.Bounds(), Proj().Img(), r, m_Transparent );
    }

	Proj().Damage( r );

    // set up for undo - want straight non-transparent blits from now on
    m_Style = FULLCOLOUR;
    m_Transparent = -1;
}

void Cmd_PlonkBrush::Undo()
{
    Do();
}
#endif
