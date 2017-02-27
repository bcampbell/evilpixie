#include "cmd.h"
#include "anim.h"
#include "draw.h"
#include "sheet.h"
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
    Proj().Damage( m_Frame, dirty );
    SetState( DONE );
}

void Cmd_Draw::Undo()
{
    assert( State() == DONE );
    Box dirty( m_Affected );
    BlitSwap( m_Img, m_Img.Bounds(), Proj().GetAnim().GetFrame(m_Frame), dirty );
    Proj().Damage( m_Frame, dirty );
    SetState( NOT_DONE );
}


Cmd_Resize::Cmd_Resize(Project& proj, Box const& new_area, int framefirst, int framelast, PenColour const& fillpen) :
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
        dest_img->FillBox(fillpen,foo);
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
    int n;
    for( n=m_First; n<m_Last; ++n)
    {
        Proj().Damage(n, b1);
    }
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



//-----------
//
Cmd_ToSpriteSheet::Cmd_ToSpriteSheet(Project& proj, int nWide) :
    Cmd(proj,NOT_DONE),
    m_NumFrames(proj.NumFrames()),
    m_NWide(nWide)
{
    if( m_NWide>m_NumFrames)
    {
        m_NWide = m_NumFrames;
    }
}

Cmd_ToSpriteSheet::~Cmd_ToSpriteSheet()
{
}



void Cmd_ToSpriteSheet::Do()
{
    Anim& anim = Proj().GetAnim();
    Img* sheet = GenerateSpriteSheet(anim,m_NWide);
    m_NumFrames = anim.NumFrames();

    anim.Zap();
    anim.Append(sheet);

    Proj().Damage_AnimReplaced();
    SetState( DONE );
}


void Cmd_ToSpriteSheet::Undo()
{
    Anim& anim = Proj().GetAnim();
    assert(anim.NumFrames()==1);
    Img const& src = anim.GetFrameConst(0);

    std::vector<Img*> frames;
    FramesFromSpriteSheet(src,m_NWide,m_NumFrames, frames);
    anim.Zap();
    unsigned int n;
    for(n=0;n<frames.size();++n)
    {
        anim.Append(frames[n]);
    }
    Proj().Damage_AnimReplaced();
    SetState( NOT_DONE );
}

//-----------
//
Cmd_FromSpriteSheet::Cmd_FromSpriteSheet(Project& proj, int nWide, int numFrames) :
    Cmd(proj,NOT_DONE),
    m_NumFrames(numFrames),
    m_NWide(nWide)
{
    if( m_NWide>m_NumFrames)
    {
        m_NWide = m_NumFrames;
    }
}

Cmd_FromSpriteSheet::~Cmd_FromSpriteSheet()
{
}



void Cmd_FromSpriteSheet::Do()
{
    Anim& anim = Proj().GetAnim();
    assert(anim.NumFrames()==1);
    Img const& src = anim.GetFrameConst(0);

    std::vector<Img*> frames;
    FramesFromSpriteSheet(src,m_NWide,m_NumFrames, frames);
    anim.Zap();
    unsigned int n;
    for(n=0;n<frames.size();++n)
    {
        anim.Append(frames[n]);
    }
    Proj().Damage_AnimReplaced();
    SetState( DONE );
}

void Cmd_FromSpriteSheet::Undo()
{
    Anim& anim = Proj().GetAnim();
    Img* sheet = GenerateSpriteSheet(anim,m_NWide);
    m_NumFrames = anim.NumFrames();

    anim.Zap();
    anim.Append(sheet);

    Proj().Damage_AnimReplaced();
    SetState( NOT_DONE );
}

//-----------
//
Cmd_PaletteModify::Cmd_PaletteModify(Project& proj, int first, int cnt, Colour const* colours) :
    Cmd(proj,NOT_DONE),
    m_First(first),
    m_Cnt(cnt)
{
    m_Colours = new Colour[cnt];
    int i;
    for (i=0; i<cnt; ++i )
        m_Colours[i] = colours[i];
}

Cmd_PaletteModify::~Cmd_PaletteModify()
{
    delete [] m_Colours;
}


void Cmd_PaletteModify::swap()
{
    Palette& pal = Proj().GetAnim().GetPalette();
    int i;
    for (i=0; i<m_Cnt; ++i)
    {
        Colour tmp = pal.GetColour(m_First+i);
        pal.SetColour(m_First+i, m_Colours[i]);
        m_Colours[i] = tmp;
    }

    Proj().Damage_Palette(m_First, m_Cnt);
}

void Cmd_PaletteModify::Do()
{
    swap();
    SetState( DONE );
}

void Cmd_PaletteModify::Undo()
{
    swap();
    SetState( NOT_DONE );
}

// attempts to merge a single colour change into the existing cmd.
// this lets us keep the project up-to-date as user twiddles the colour,
// but also avoids clogging up the undo stack with insane numbers of operations.
// returns true if the merge occured, false if a new cmd is required.
bool Cmd_PaletteModify::Merge( int idx, Colour const& newc)
{
    if (m_Cnt!=1 || m_First!=idx)
        return false;
    if (State()!=DONE)
        return false;

    Palette& pal = Proj().GetAnim().GetPalette();
    pal.SetColour(m_First, newc);
    Proj().Damage_Palette(m_First, m_Cnt);
    return true;
}


//-----------
//
//

Cmd_Batch::Cmd_Batch( Project& proj, CmdState initialstate) :
    Cmd(proj,initialstate)
{
}

Cmd_Batch::~Cmd_Batch()
{
    while (!m_Cmds.empty()) {
        delete m_Cmds.back();
        m_Cmds.pop_back();
    }
}


void Cmd_Batch::Append(Cmd* c)
{
    assert(c->State() == State());
    m_Cmds.push_back(c);
}


void Cmd_Batch::Do()
{
    std::vector<Cmd*>::iterator it;
    for (it=m_Cmds.begin(); it!=m_Cmds.end(); ++it) {
        (*it)->Do(); 
    }
}

void Cmd_Batch::Undo()
{
    std::vector<Cmd*>::reverse_iterator it;
    for (it=m_Cmds.rbegin(); it!=m_Cmds.rend(); ++it) {
        (*it)->Undo(); 
    }
}

