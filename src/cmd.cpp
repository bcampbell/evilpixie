#include "cmd.h"
#include "anim.h"
#include "project.h"
#include <assert.h>
#include <cstdio>

Cmd_Draw::Cmd_Draw( Project& proj, int frame, Box const& affected, Img const& undoimg ) :
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
    BlitSwap( m_Img, m_Img.Bounds(), Proj().GetAnim().GetFrame(m_Frame), dirty );
    Proj().Damage( dirty );
    SetState( DONE );
}

void Cmd_Draw::Undo()
{
    assert( State() == DONE );
    Box dirty( m_Affected );
    BlitSwap( m_Img, m_Img.Bounds(), Proj().GetAnim().GetFrame(m_Frame), dirty );
    Proj().Damage( dirty );
    SetState( NOT_DONE );
}




Cmd_Resize::Cmd_Resize(Project& proj, Box const& new_area, int framefirst, int framelast) :
    Cmd(proj,NOT_DONE),
    m_First(framefirst),
    m_Last(framelast)
{
    // populate frameswap with the resized frames
    int n;
    for(n=m_First; n<m_Last; ++n)
    {
        Img const& src_img = Proj().GetAnim().GetFrameConst(n);
        Img* dest_img = new Img(src_img.Fmt(), new_area.w, new_area.h);
        Box foo(dest_img->Bounds());
        dest_img->FillBox(Proj().BGPen(),foo);
        Box src_area(src_img.Bounds());
        Box dest_area(src_area);
        dest_area -= new_area.TopLeft();

        Blit(src_img, src_area, *dest_img, dest_area);
        m_FrameSwap.Append(dest_img);
    }
}


Cmd_Resize::~Cmd_Resize()
{
}


void Cmd_Resize::Swap()
{
    Anim tmp;
    Proj().GetAnim().TransferFrames(m_First, m_Last, tmp, 0);
    m_FrameSwap.TransferFrames(0,m_FrameSwap.NumFrames(), Proj().GetAnim(), m_First);
    assert(m_FrameSwap.NumFrames()==0);
    tmp.TransferFrames(0,tmp.NumFrames(),m_FrameSwap,0);

    Box b1,b2;
    Proj().GetAnim().CalcBounds(b1, m_First, m_Last);
    m_FrameSwap.CalcBounds(b2, 0, m_FrameSwap.NumFrames());
    b1.Merge(b2);
    Proj().Damage(b1);
}

void Cmd_Resize::Do()
{
    Swap();
    SetState( DONE );
}

void Cmd_Resize::Undo()
{
    Swap();
    SetState( NOT_DONE );
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
    Img const& srcimg(Proj().GetAnim().GetFrameConst(m_Pos));
    Anim tmp;
    int i;
    for(i=0;i<m_Num;++i)
        tmp.Append(new Img(srcimg));

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





Cmd_DeleteFrames::Cmd_DeleteFrames(Project& proj, int first, int last) :
    Cmd(proj,NOT_DONE),
    m_First(first),
    m_Last(last)
{
}

Cmd_DeleteFrames::~Cmd_DeleteFrames()
{
}



void Cmd_DeleteFrames::Do()
{
    Proj().GetAnim().TransferFrames(m_First, m_Last, m_FrameSwap,0);
    Proj().Damage_FramesRemoved(m_First,m_Last);
    SetState( DONE );
}


void Cmd_DeleteFrames::Undo()
{
    m_FrameSwap.TransferFrames( 0,m_FrameSwap.NumFrames(), Proj().GetAnim(), m_First);
    Proj().Damage_FramesAdded(m_First,m_Last);
    SetState( NOT_DONE );
}

