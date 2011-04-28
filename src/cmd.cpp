#include "cmd.h"
#include "anim.h"
#include "project.h"
#include <assert.h>
#include <cstdio>

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



Cmd_InsertFrames::Cmd_InsertFrames(Project& proj, int position, int numframes) :
    Cmd(proj,NOT_DONE),
    m_Pos(position),
    m_Num(numframes)
{
}

Cmd_InsertFrames::~Cmd_InsertFrames()
{
}



void Cmd_InsertFrames::Do()
{
    IndexedImg const& srcimg(Proj().GetAnim().GetFrameConst(m_Pos));
    Anim tmp;
    int i;
    for(i=0;i<m_Num;++i)
        tmp.Append( new IndexedImg(srcimg), 0 );

    tmp.TransferFrames(0,tmp.NumFrames(), Proj().GetAnim(),m_Pos+1);

    Proj().Damage_FramesAdded(m_Pos+1,m_Pos+1+m_Num);
    SetState( DONE );
}


void Cmd_InsertFrames::Undo()
{
    Anim tmp;
    Proj().GetAnim().TransferFrames(m_Pos+1,m_Pos+1+m_Num,tmp,0);
    // tmp will delete the frames as it goes out of scope
    Proj().Damage_FramesRemoved(m_Pos+1,m_Pos+1+m_Num);
    SetState( NOT_DONE );
}

