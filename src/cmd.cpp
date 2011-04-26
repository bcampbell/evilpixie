#include "cmd.h"
#include "project.h"
#include <assert.h>

Cmd_Draw::Cmd_Draw( Project& proj, int frame, Box const& affected, IndexedImg const& undoimg ) :
    Cmd( proj, DONE ),
    m_Frame(frame),
    m_Img( undoimg, affected ),
    m_Affected( affected )
{

}

void Cmd_Draw::Do()
{
    assert( State() == NOT_DONE );
    Box dirty( m_Affected );
    BlitSwapIndexed( m_Img, m_Img.Bounds(), Proj().Img(m_Frame), dirty );
    Proj().Damage( dirty );
    SetState( DONE );
}

void Cmd_Draw::Undo()
{
    assert( State() == DONE );
    Box dirty( m_Affected );
    BlitSwapIndexed( m_Img, m_Img.Bounds(), Proj().Img(m_Frame), dirty );
    Proj().Damage( dirty );
    SetState( NOT_DONE );
}




Cmd_Resize::Cmd_Resize(Project& proj, Box const& new_area) :
    Cmd(proj,NOT_DONE),
    m_Img(new IndexedImg(new_area.W(), new_area.H()))
{
#if 0
    //
    Box foo(m_Img->Bounds());
    m_Img->FillBox(Proj().BGPen(), foo);

    Box src_area(Proj().Img().Bounds());
    Box dest_area(src_area);
    dest_area -= new_area.TopLeft();

    BlitIndexed(Proj().Img(), src_area, *m_Img, dest_area, -1, -1);
#endif
}


Cmd_Resize::~Cmd_Resize()
{
    delete m_Img;
}


void Cmd_Resize::Do()
{
#if 0
    assert( State() == NOT_DONE );
    m_Img = Proj().ReplaceImg(m_Img);
    Proj().Damage(Proj().Img().Bounds());
    SetState( DONE );
#endif
}

void Cmd_Resize::Undo()
{
#if 0
    assert( State() == DONE );
    // BLAH BLAH
    SetState( NOT_DONE );
#endif
}

