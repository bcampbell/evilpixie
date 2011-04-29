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




Cmd_Resize::Cmd_Resize(Project& proj, Box const& new_area, int framefirst, int framelast) :
    Cmd(proj,NOT_DONE),
    m_First(framefirst),
    m_Last(framelast)
{
    // populate frameswap with the resized frames
    int n;
    for(n=m_First; n<m_Last; ++n)
    {
        IndexedImg* dest_img = new IndexedImg(new_area.w, new_area.h);
        Box foo(dest_img->Bounds());
        dest_img->FillBox(Proj().BGPen(),foo);
        IndexedImg const& src_img = Proj().GetAnim().GetFrameConst(n);
        Box src_area(src_img.Bounds());
        Box dest_area(src_area);
        dest_area -= new_area.TopLeft();

        BlitIndexed(src_img, src_area, *dest_img, dest_area, -1, -1);
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
}

void Cmd_Resize::Do()
{
    Swap();
    Proj().Damage(Proj().Img(m_First).Bounds());
    SetState( DONE );
}

void Cmd_Resize::Undo()
{
    Swap();
    Proj().Damage(Proj().Img(m_First).Bounds());
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
    IndexedImg const& srcimg(Proj().GetAnim().GetFrameConst(m_Pos));
    Anim tmp;
    int i;
    for(i=0;i<m_Num;++i)
        tmp.Append(new IndexedImg(srcimg));

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

